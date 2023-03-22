#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <mysqlx/xdevapi.h>
#include "spdlog/spdlog.h"

class MySQLDatabase {
public:
    MySQLDatabase(const std::string& host, const std::string& user, const std::string& password, const std::string& database_name) {
        try {
            session_ = mysqlx::getSession(host, user, password, database_name);
            session_.sql("CREATE DATABASE IF NOT EXISTS " + database_name).execute();
            session_.close();
            session_ = mysqlx::getSession(host, user, password, database_name);
        }
        catch (const mysqlx::Error& e) {
            spdlog::error("Failed to create/connect to database: {}", e.what());
        }
    }

    ~MySQLDatabase() {
        if (session_.isValid()) {
            session_.close();
        }
    }

    void execute(const std::string& query) {
        try {
            session_.sql(query).execute();
        }
        catch (const mysqlx::Error& e) {
            spdlog::error("Error executing query \"{}\": {}", query, e.what());
        }
    }

    void insert(const std::string& table_name, const std::vector<std::string>& field_names, const std::vector<std::vector<std::string>>& values) {
        try {
            mysqlx::Table table = session_.getSchema().getTable(table_name);
            for (const auto& row : values) {
                mysqlx::RowValues row_values;
                for (size_t i = 0; i < field_names.size(); ++i) {
                    row_values.append(row[i]);
                }
                table.insert(field_names).values(row_values).execute();
            }
        }
        catch (const mysqlx::Error& e) {
            spdlog::error("Error inserting into table \"{}\": {}", table_name, e.what());
        }
    }

    mysqlx::Result select(const std::string& query) {
        return session_.sql(query).execute();
    }

    void remove(const std::string& table_name, const std::string& filter) {
        try {
            mysqlx::Table table = session_.getSchema().getTable(table_name);
            table.remove().where(filter).execute();
        }
        catch (const mysqlx::Error& e) {
            spdlog::error("Error removing from table \"{}\": {}", table_name, e.what());
        }
    }

    void create_table(const std::string& table_name, const std::vector<std::string>& field_names) {
        std::string create_query = "CREATE TABLE IF NOT EXISTS " + table_name + " (";
        for (size_t i = 0; i < field_names.size(); ++i) {
            create_query += field_names[i];
            if (i < field_names.size() - 1) {
                create_query += ",";
            }
        }
        create_query += ")";
        execute(create_query);
    }

    void optimize(const std::string& table_name) {
        execute("OPTIMIZE TABLE " + table_name);
    }

private:
    mysqlx::Session session_;
};

int main() {
    const std::string host = "localhost";
    const std::string user = "root";
    const std::string password = "password";
    const std::string database_name = "my_database";
    const std::string table_name = "my_table";
    const std::vector<std::string> field_names = {"id", "name", "age"};

    // Check if MySQL server is installed
    std::ifstream infile("/usr/sbin/mysqld");
    if (!infile.good()) {
        spdlog::error("MySQL server is not installed");
        return 1;
    }

    // Check if database path exists
    namespace fs = std::filesystem;
    fs::path database_path = "/var/lib/mysql/" + database_name;
    if (!fs::exists(database_path)) {
        spdlog::error("Database path does not exist: {}", database_path.string());
        return 1;
    }

    // Create and connect to the database
    MySQLDatabase database(host, user, password, database_name);

    // Create table
    database.create_table(table_name, field_names);

    // Insert data
    const std::vector<std::vector<std::string>> values = {{"1", "Alice", "22"}, {"2", "Bob", "30"}, {"3", "Charlie", "27"}};
    database.insert(table_name, field_names, values);

    // Select data
    mysqlx::Result result = database.select("SELECT * FROM " + table_name);
    for (auto row : result.getRows()) {
        std::cout << row[0] << " " << row[1] << " " << row[2] << std::endl;
    }

    // Remove data
    database.remove(table_name, "age > 25");

    // Filter and sort data
    result = database.select("SELECT * FROM " + table_name + " WHERE age < 30 ORDER BY age DESC");
    for (auto row : result.getRows()) {
        std::cout << row[0] << " " << row[1] << " " << row[2] << std::endl;
    }

    // Optimize table
    database.optimize(table_name);

    // Close the session
    database.~MySQLDatabase();

    return 0;
}
