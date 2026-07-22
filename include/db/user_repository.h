#pragma once
#include <optional>
#include <string>
#include <sqlite3.h>

struct UserRecord {
    std::string username;
    std::string passwordHash;
    std::string salt;
    int rating;
};

class UserRepository {
public:
    explicit UserRepository(const std::string& dbPath);
    ~UserRepository();

    UserRepository(const UserRepository&) = delete;
    UserRepository& operator=(const UserRepository&) = delete;

    std::optional<UserRecord> findByUsername(const std::string& username);
    void create(const std::string& username, const std::string& passwordHash, const std::string& salt);

private:
    sqlite3* db_ = nullptr;
};
