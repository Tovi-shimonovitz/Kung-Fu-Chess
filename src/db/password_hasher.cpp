#include "../../include/db/password_hasher.h"
#include <websocketpp/sha1/sha1.hpp>
#include <random>
#include <sstream>
#include <iomanip>

std::string PasswordHasher::generateSalt() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist;

    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(16) << dist(gen);
    return oss.str();
}

std::string PasswordHasher::hash(const std::string& password, const std::string& salt) {
    std::string combined = salt + password;
    unsigned char digest[20];
    websocketpp::sha1::calc(combined.data(), combined.size(), digest);

    std::ostringstream oss;
    for (unsigned char byte : digest) {
        oss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
    }
    return oss.str();
}
