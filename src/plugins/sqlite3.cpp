#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>
#include <fstream>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/istreamwrapper.h>
#include <boost/algorithm/string.hpp>

class SQLiteManager {
public:
    SQLiteManager(const std::string& dbPath) : m_db(nullptr) {
        int ret = sqlite3_open(dbPath.c_str(), &m_db);
        if (ret != SQLITE_OK) {
            std::cerr << "Cannot open database: " << sqlite3_errmsg(m_db) << std::endl;
            sqlite3_close(m_db);
            m_db = nullptr;
        }
    }

    ~SQLiteManager() {
        if (m_db) {
            sqlite3_close(m_db);
        }
    }

    bool executeSQL(const std::string& sql, sqlite3_callback callback = nullptr) {
        char* errMsg;
        int ret = sqlite3_exec(m_db, sql.c_str(), callback, nullptr, &errMsg);
        if (ret != SQLITE_OK) {
            std::cerr << "Error executing SQL: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
        return true;
    }

    bool search(const std::string& tableName, const std::string& columnName, const std::string& keyword,
                std::vector<std::vector<std::string>>& results) {
        std::string sql = "SELECT * FROM " + tableName + " WHERE " + columnName + " LIKE '%" + keyword + "%'";
        auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
            auto results = static_cast<std::vector<std::vector<std::string>>*>(data);
            std::vector<std::string> row;
            for (int i = 0; i < argc; ++i) {
                row.push_back(argv[i] ? argv[i] : "NULL");
            }
            results->push_back(row);
            return 0;
        };
        results.clear();
        return executeSQL(sql, callback) && !results.empty();
    }

    bool addRecord(const std::string& tableName, const std::vector<std::string>& values) {
        std::string sql = "INSERT INTO " + tableName + " VALUES (NULL, '" + join(values, "', '") + "')";
        return executeSQL(sql);
    }

    bool deleteRecord(const std::string& tableName, const std::string& condition) {
        std::string sql = "DELETE FROM " + tableName + " WHERE " + condition;
        return executeSQL(sql);
    }

    bool filter(const std::string& tableName, const std::string& condition,
                std::vector<std::vector<std::string>>& results) {
        std::string sql = "SELECT * FROM " + tableName + " WHERE " + condition;
        auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
            auto results = static_cast<std::vector<std::vector<std::string>>*>(data);
            std::vector<std::string> row;
            for (int i = 0; i < argc; ++i) {
                row.push_back(argv[i] ? argv[i] : "NULL");
            }
            results->push_back(row);
            return 0;
        };
        results.clear();
        return executeSQL(sql, callback) && !results.empty();
    }

    bool sort(const std::string& tableName, const std::string& columnName, bool ascending,
              std::vector<std::vector<std::string>>& results) {

        std::string sql = "SELECT * FROM " + tableName + " ORDER BY " + columnName;
        if (!ascending) {
            sql += " DESC";
        }
        auto callback = [](void* data, int argc, char** argv, char** colName) -> int {
            auto results = static_cast<std::vector<std::vector<std::string>>*>(data);
            std::vector<std::string> row;
            for (int i = 0; i < argc; ++i) {
                row.push_back(argv[i] ? argv[i] : "NULL");
            }
            results->push_back(row);
            return 0;
        };
        results.clear();

        return executeSQL(sql, callback) && !results.empty();
    }

    bool exportToJson(const std::string& tableName, const std::string& jsonPath) {
        std::string sql = "SELECT * FROM " + tableName;
        sqlite3_stmt* stmt = nullptr;
        int ret = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
        if (ret != SQLITE_OK) {
            std::cerr << "Error preparing SQL: " << sqlite3_errmsg(m_db) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }

        std::vector<std::string> colNames;
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            colNames.push_back(sqlite3_column_name(stmt, i));
        }

        rapidjson::Document document;
        document.SetArray();
        while ((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
            rapidjson::Value row(rapidjson::kArrayType);
            for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
                const char* name = sqlite3_column_name(stmt, i);
                if (sqlite3_column_type(stmt, i) == SQLITE_NULL) {
                    row.PushBack(rapidjson::Value().SetNull(), document.GetAllocator());
                } else if (sqlite3_column_type(stmt, i) == SQLITE_INTEGER) {
                    int value = sqlite3_column_int(stmt, i);
                    row.PushBack(rapidjson::Value(value), document.GetAllocator());
                } else if (sqlite3_column_type(stmt, i) == SQLITE_FLOAT) {
                    double value = sqlite3_column_double(stmt, i);
                    row.PushBack(rapidjson::Value(value), document.GetAllocator());
                } else {
                    const char* value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                    row.PushBack(rapidjson::Value().SetString(value, strlen(value), document.GetAllocator()));
                }
            }
            document.PushBack(row, document.GetAllocator());
        }
        sqlite3_finalize(stmt);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);

        std::ofstream outFile(jsonPath);
        if (!outFile.is_open()) {
            std::cerr << "Error opening file " << jsonPath << std::endl;
            return false;
        }
        outFile << buffer.GetString();
        outFile.close();
        return true;
    }

    bool exportToCsv(const std::string& tableName, const std::string& csvPath) {
        std::string sql = "SELECT * FROM " + tableName;
        sqlite3_stmt* stmt = nullptr;
        int ret = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
        if (ret != SQLITE_OK) {
            std::cerr << "Error preparing SQL: " << sqlite3_errmsg(m_db) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }

        std::vector<std::string> colNames;
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            colNames.push_back(sqlite3_column_name(stmt, i));
        }

        std::ofstream outFile(csvPath);
        if (!outFile.is_open()) {
            std::cerr << "Error opening file " << csvPath << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }

        for (int i = 0; i < colNames.size(); ++i) {
            outFile << colNames[i];
            if (i != colNames.size() - 1) {
                outFile << ",";
            }
        }
        outFile << std::endl;

        while ((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
            for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
                if (i != 0) {
                    outFile << ",";
                }
                const char* value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                outFile << "\"" << value << "\"";
            }
            outFile << std::endl;
        }
        sqlite3_finalize(stmt);

        outFile.close();
        return true;
    }

    bool importFromJson(const std::string& tableName, const std::string& jsonPath) {
        std::ifstream inFile(jsonPath);
        if (!inFile.is_open()) {
            std::cerr << "Error opening file " << jsonPath << std::endl;
            return false;
        }

        rapidjson::IStreamWrapper wrapper(inFile);
        rapidjson::Document document;
        document.ParseStream(wrapper);

        std::vector<std::string> colNames;
        if (document.Size() > 0) {
            for (const auto& member : document[0].GetObject()) {
                colNames.push_back(member.name.GetString());
            }
        }

        std::string createTableSql = "CREATE TABLE IF NOT EXISTS " + tableName + " (";
        for (int i = 0; i < colNames.size(); ++i) {
            createTableSql += colNames[i] + " TEXT";
            if (i != colNames.size() - 1) {
                createTableSql += ",";
            }
        }
        createTableSql += ");";

        if (!executeSQL(createTableSql)) {
            std::cerr << "Error creating table " << tableName << std::endl;
            return false;
        }

        std::string insertSql = "INSERT INTO " + tableName + " VALUES (";
        for (int i = 0; i < colNames.size(); ++i) {
            insertSql += "?";
            if (i != colNames.size() - 1) {
                insertSql += ",";
            }
        }
        insertSql += ");";

        sqlite3_stmt* stmt = nullptr;
        int ret = sqlite3_prepare_v2(m_db, insertSql.c_str(), -1, &stmt, nullptr);
        if (ret != SQLITE_OK) {
            std::cerr << "Error preparing SQL: " << sqlite3_errmsg(m_db) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }

        for (const auto& row : document.GetArray()) {
            int index = 1;
            for (const auto& member : row.GetObject()) {
                const char* value = member.value.GetString();
                sqlite3_bind_text(stmt, index++, value, -1, SQLITE_TRANSIENT);
            }
            ret = sqlite3_step(stmt);
            if (ret != SQLITE_DONE) {
                std::cerr << "Error inserting record: " << sqlite3_errmsg(m_db) << std::endl;
                sqlite3_finalize(stmt);
                return false;
            }
            sqlite3_clear_bindings(stmt);
            sqlite3_reset(stmt);
        }

        sqlite3_finalize(stmt);
        inFile.close();
        return true;
    }

    bool importFromCsv(const std::string& tableName, const std::string& csvPath) {
        std::ifstream inFile(csvPath);
        if (!inFile.is_open()) {
            std::cerr << "Error opening file " << csvPath << std::endl;
            return false;
        }

        std::string line;
        std::getline(inFile, line);
        std::vector<std::string> colNames;
        boost::split(colNames, line, boost::is_any_of(","));

        std::string createTableSql = "CREATE TABLE IF NOT EXISTS " + tableName + " (";
        for (int i = 0; i < colNames.size(); ++i) {
            createTableSql += colNames[i] + " TEXT";
            if (i != colNames.size() - 1) {
                createTableSql += ",";
            }
        }
        createTableSql += ");";

        if (!executeSQL(createTableSql)) {
            std::cerr << "Error creating table " << tableName << std::endl;
            return false;
        }

        std::string insertSql = "INSERT INTO " + tableName + " VALUES (";
        for (int i = 0; i < colNames.size(); ++i) {
            insertSql += "?";
            if (i != colNames.size() - 1) {
                insertSql += ",";
            }
        }
        insertSql += ");";

        sqlite3_stmt* stmt = nullptr;
        int ret = sqlite3_prepare_v2(m_db, insertSql.c_str(), -1, &stmt, nullptr);
        if (ret != SQLITE_OK) {
            std::cerr << "Error preparing SQL: " << sqlite3_errmsg(m_db) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }

        while (std::getline(inFile, line)) {
            std::vector<std::string> row;
            boost::split(row, line, boost::is_any_of(","));

            int index = 1;
            for (const auto& value : row) {
                sqlite3_bind_text(stmt, index++, value.c_str(), -1, SQLITE_TRANSIENT);
            }
            ret = sqlite3_step(stmt);
            if (ret != SQLITE_DONE) {
                std::cerr << "Error inserting record: " << sqlite3_errmsg(m_db) << std::endl;
                sqlite3_finalize(stmt);
                return false;
            }
            sqlite3_clear_bindings(stmt);
            sqlite3_reset(stmt);
        }

        sqlite3_finalize(stmt);
        inFile.close();
        return true;
    }

private:
    std::string join(const std::vector<std::string>& strs, const std::string& separator) {
        std::string result;
        for (int i = 0; i < strs.size(); ++i) {
            result += strs[i];
            if (i != strs.size() - 1) {
                result += separator;
            }
        }
        return result;
    }

    sqlite3* m_db;
};

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <database_path>" << std::endl;
        return 1;
    }

    SQLiteManager db(argv[1]);

    if (!db.executeSQL("CREATE TABLE IF NOT EXISTS person (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)")) {
        return 1;
    }

    std::vector<std::string> record = {"Alice", "20"};
    if (!db.addRecord("person", record)) {
        return 1;
    }

    std::string keyword = "li";
    std::vector<std::vector<std::string>> results;
    if (db.search("person", "name", keyword, results)) {
        for (const auto& row : results) {
            std::cout << "id: " << row[0] << ", name: " << row[1] << ", age: " << row[2] << std::endl;
        }
    }

    std::string condition = "age > 18";
    if (db.filter("person", condition, results)) {
        for (const auto& row : results) {
            std::cout << "id: " << row[0] << ", name: " << row[1] << ", age: " << row[2] << std::endl;
        }
    }

    std::string columnName = "age";
    if (db.sort("person", columnName, false, results)) {
        for (const auto& row : results) {
            std::cout << "id: " << row[0] << ", name: " << row[1] << ", age: " << row[2] << std::endl;
        }
    }

    std::string deleteCondition = "name='Alice'";
    if (!db.deleteRecord("person", deleteCondition)) {
        return 1;
    }

    return 0;
}
