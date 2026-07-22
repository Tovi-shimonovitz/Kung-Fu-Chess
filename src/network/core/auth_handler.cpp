#include "../../../include/network/core/auth_handler.h"
#include "../../../include/db/user_repository.h"
#include "../../../include/db/password_hasher.h"

AuthHandler::AuthHandler(UserRepository& userRepository) : userRepository_(userRepository) {}

std::optional<std::string> AuthHandler::authenticate(const std::string& username, const std::string& password) {
    auto existing = userRepository_.findByUsername(username);
    if (existing) {
        std::string attemptedHash = PasswordHasher::hash(password, existing->salt);
        if (attemptedHash != existing->passwordHash) {
            return "ERROR: incorrect password";
        }
        return std::nullopt;
    }

    std::string salt = PasswordHasher::generateSalt();
    std::string hash = PasswordHasher::hash(password, salt);
    userRepository_.create(username, hash, salt);
    return std::nullopt;
}
