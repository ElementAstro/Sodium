#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>
#include <filesystem>
#include "spdlog/spdlog.h"

class SqliteDB
{
public:
    SqliteDB() : db(nullptr){}

    bool open(const std::string& path = "./data.db")
    {
        // 检查路径是否存在
        std::filesystem::path p(path);
        if (!std::filesystem::exists(p.parent_path()))
        {
            spdlog::error("Path does not exist: {}", p.parent_path().string());
            return false;
        }

        // 检查数据库是否安装
        int version = sqlite3_libversion_number();
        if (version < 3008000)
        {
            spdlog::error("SQLite version {} is too old, please upgrade to at least 3.8.0", version);
            return false;
        }

        // 打开数据库
        int rc = sqlite3_open(path.c_str(), &db);

        if (rc != SQLITE_OK)
        {
            spdlog::error("Cannot open database: {}", sqlite3_errmsg(db));
            sqlite3_close(db);
            db = nullptr;
            return false;
        }
        else
        {
            spdlog::info("Database opened successfully: {}", path);
            return true;
        }
    }

    void close()
    {
        if (db != nullptr)
        {
            sqlite3_close(db);
            spdlog::info("Database closed");
        }
    }

    bool create_table(const std::string& table_name, const std::vector<std::string>& columns)
    {
        if (db == nullptr) return false;

        std::string sql = "CREATE TABLE IF NOT EXISTS " + table_name + " (";
        for (const auto& col : columns)
        {
            sql += col + ",";
        }
        sql.pop_back(); // 去除最后一个逗号
        sql += ")";

        char* errmsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errmsg);
        if (rc != SQLITE_OK)
        {
            spdlog::error("Failed to create table {}: {}", table_name, errmsg);
            sqlite3_free(errmsg);
            return false;
        }
        else
        {
            spdlog::info("Table {} created successfully", table_name);
            return true;
        }
    }

    bool insert(const std::string& table_name, const std::vector<std::string>& values)
    {
        if (db == nullptr) return false;

        std::string sql = "INSERT INTO " + table_name + " VALUES (";
        for (const auto& val : values)
        {
            sql += "'" + val + "',";
        }
        sql.pop_back(); // 去除最后一个逗号
        sql += ")";

        char* errmsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errmsg);
        if (rc != SQLITE_OK)
        {
            spdlog::error("Failed to insert into {}: {}", table_name, errmsg);
            sqlite3_free(errmsg);
            return false;
        }
        else
        {
            spdlog::info("Inserted successfully into {}", table_name);
            return true;
        }
    }

    bool update(const std::string& table_name, const std::string& column, const std::string& value,
                const std::string& where_column, const std::string& where_value)
    {
        if (db == nullptr) return false;

        std::string sql = "UPDATE " + table_name + " SET " + column + "='" + value + "' WHERE "
        + where_column + "='" + where_value + "'";

        char* errmsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errmsg);
        if (rc != SQLITE_OK)
        {
            spdlog::error("Failed to update {}: {}", table_name, errmsg);
            sqlite3_free(errmsg);
            return false;
        }
        else
        {
            spdlog::info("Updated successfully in {}", table_name);
            return true;
        }
    }

    bool del(const std::string& table_name, const std::string& where_column, const std::string& where_value)
    {
        if (db == nullptr) return false;

        std::string sql = "DELETE FROM " + table_name + " WHERE " + where_column + "='" + where_value + "'";

        char* errmsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errmsg);
        if (rc != SQLITE_OK)
        {
            spdlog::error("Failed to delete from {}: {}", table_name, errmsg);
            sqlite3_free(errmsg);
            return false;
        }
        else
        {
            spdlog::info("Deleted successfully from {}", table_name);
            return true;
        }
    }

    bool select_all(const std::string& table_name, std::vector<std::vector<std::string>>& results)
    {
        if (db == nullptr) return false;

        std::string sql = "SELECT * FROM " + table_name;

        char* errmsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), callback, &results, &errmsg);
        if (rc != SQLITE_OK)
        {
            spdlog::error("Failed to select from {}: {}", table_name, errmsg);
            sqlite3_free(errmsg);
            return false;
        }
        else
        {
            spdlog::info("Selected successfully from {}", table_name);
            return true;
        }
    }

    bool filter(const std::string& table_name, const std::string& column, const std::string& value,
                std::vector<std::vector<std::string>>& results)
    {
        if (db == nullptr) return false;

        std::string sql = "SELECT * FROM " + table_name + " WHERE " + column + "='" + value + "'";

        char* errmsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), callback, &results, &errmsg);
        if (rc != SQLITE_OK)
        {
            spdlog::error("Failed to filter {}: {}", table_name, errmsg);
            sqlite3_free(errmsg);
            return false;
        }
        else
        {
            spdlog::info("Filtered successfully from {}", table_name);
            return true;
        }
    }

    bool sort(const std::string& table_name, const std::string& column, bool ascending,
              std::vector<std::vector<std::string>>& results)
    {
        if (db == nullptr) return false;

        std::string sql = "SELECT * FROM " + table_name + " ORDER BY " + column
                          + (ascending ? " ASC" : " DESC");

        char* errmsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), callback, &results, &errmsg);
        if (rc != SQLITE_OK)
        {
            spdlog::error("Failed to sort {}: {}", table_name, errmsg);
            sqlite3_free(errmsg);
            return false;
        }
        else
        {
            spdlog::info("Sorted successfully from {}", table_name);
            return true;
        }
    }

private:
    sqlite3* db;

    static int callback(void* data, int argc, char** argv, char** azColName)
    {
        auto results = static_cast<std::vector<std::vector<std::string>>*>(data);

        std::vector<std::string> row;
        for(int i=0; i<argc; i++)
        {
            row.push_back(argv[i]);
        }

        results->push_back(row);
        return 0;
    }
};

/*
int main()
{
    SqliteDB db;
    if (db.open())
    {
        std::vector<std::string> columns = {"id INTEGER PRIMARY KEY", "name TEXT", "age INT"};
        db.create_table("students", columns);

        std::vector<std::string> values1 = {"1", "Tom", "18"};
        std::vector<std::string> values2 = {"2", "Jerry", "20"};
        db.insert("students", values1);
        db.insert("students", values2);

        std::vector<std::vector<std::string>> results;
        db.select_all("students", results);
        for (const auto& row : results)
        {
            for (const auto& col : row)
            {
                std::cout << col << "\t";
            }
            std::cout << std::endl;
        }

        db.close();
    }

    return 0;
}
*/
