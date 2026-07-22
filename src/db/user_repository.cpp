#include "../../include/db/user_repository.h"
#include <stdexcept>

UserRepository::UserRepository(const std::string& dbPath) {
    if (sqlite3_open(dbPath.c_str(), &db_) != SQLITE_OK) {
        std::string message = sqlite3_errmsg(db_);
        sqlite3_close(db_);
        throw std::runtime_error("ERROR CANNOT_OPEN_DB: " + message);
    }

    const char* createTableSql =
        "CREATE TABLE IF NOT EXISTS users ("
        "username TEXT PRIMARY KEY,"
        "password_hash TEXT NOT NULL,"
        "salt TEXT NOT NULL,"
        "rating INTEGER NOT NULL DEFAULT 1200"
        ");";
    char* errMsg = nullptr;
    if (sqlite3_exec(db_, createTableSql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string message = errMsg ? errMsg : "unknown error";
        sqlite3_free(errMsg);
        sqlite3_close(db_);
        throw std::runtime_error("ERROR CREATE_TABLE_FAILED: " + message);
    }
}

UserRepository::~UserRepository() {
    sqlite3_close(db_);
}

std::optional<UserRecord> UserRepository::findByUsername(const std::string& username) {
    const char* sql = "SELECT username, password_hash, salt, rating FROM users WHERE username = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("ERROR PREPARE_FAILED: " + std::string(sqlite3_errmsg(db_)));
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<UserRecord> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        UserRecord record;
        record.username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        record.passwordHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        record.salt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        record.rating = sqlite3_column_int(stmt, 3);
        result = record;
    }

    sqlite3_finalize(stmt);
    return result;
}

void UserRepository::create(const std::string& username, const std::string& passwordHash, const std::string& salt) {
    const char* sql = "INSERT INTO users (username, password_hash, salt, rating) VALUES (?, ?, ?, 1200);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("ERROR PREPARE_FAILED: " + std::string(sqlite3_errmsg(db_)));
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, salt.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("ERROR INSERT_FAILED: " + std::string(sqlite3_errmsg(db_)));
    }

    sqlite3_finalize(stmt);
}
