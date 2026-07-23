#pragma once
#include <optional>
#include <string>

class UserRepository;

class AuthHandler {
public:
    explicit AuthHandler(UserRepository& userRepository);

    std::optional<std::string> authenticate(const std::string& username, const std::string& password);
    int ratingOf(const std::string& username);

private:
    UserRepository& userRepository_;
};
