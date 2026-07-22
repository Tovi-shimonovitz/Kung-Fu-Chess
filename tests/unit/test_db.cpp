#include <cassert>
#include <cstdio>
#include <iostream>
#include "../../include/db/password_hasher.h"
#include "../../include/db/user_repository.h"

void log_test(const std::string& name) {
    std::cout << "[PASSED] " << name << std::endl;
}

void test_generate_salt_produces_different_values() {
    std::string salt1 = PasswordHasher::generateSalt();
    std::string salt2 = PasswordHasher::generateSalt();
    assert(salt1 != salt2);
    assert(!salt1.empty());
    log_test("test_generate_salt_produces_different_values");
}

void test_hash_is_deterministic_for_same_password_and_salt() {
    std::string salt = PasswordHasher::generateSalt();
    std::string hash1 = PasswordHasher::hash("mypassword", salt);
    std::string hash2 = PasswordHasher::hash("mypassword", salt);
    assert(hash1 == hash2);
    log_test("test_hash_is_deterministic_for_same_password_and_salt");
}

void test_hash_differs_with_different_salt() {
    std::string hash1 = PasswordHasher::hash("mypassword", "salt_a");
    std::string hash2 = PasswordHasher::hash("mypassword", "salt_b");
    assert(hash1 != hash2);
    log_test("test_hash_differs_with_different_salt");
}

void test_hash_differs_with_different_password() {
    std::string salt = "same_salt";
    std::string hash1 = PasswordHasher::hash("password_one", salt);
    std::string hash2 = PasswordHasher::hash("password_two", salt);
    assert(hash1 != hash2);
    log_test("test_hash_differs_with_different_password");
}

namespace {
const char* TEST_DB_PATH = "test_user_repository.db";
}

void test_create_and_find_round_trip() {
    std::remove(TEST_DB_PATH);
    UserRepository repo(TEST_DB_PATH);

    std::string salt = PasswordHasher::generateSalt();
    std::string hash = PasswordHasher::hash("secret", salt);
    repo.create("alice", hash, salt);

    auto found = repo.findByUsername("alice");
    assert(found.has_value());
    assert(found->username == "alice");
    assert(found->passwordHash == hash);
    assert(found->salt == salt);
    assert(found->rating == 1200);

    std::remove(TEST_DB_PATH);
    log_test("test_create_and_find_round_trip");
}

void test_find_missing_user_returns_nullopt() {
    std::remove(TEST_DB_PATH);
    UserRepository repo(TEST_DB_PATH);

    auto found = repo.findByUsername("nobody");
    assert(!found.has_value());

    std::remove(TEST_DB_PATH);
    log_test("test_find_missing_user_returns_nullopt");
}

void test_two_users_are_independent() {
    std::remove(TEST_DB_PATH);
    UserRepository repo(TEST_DB_PATH);

    repo.create("alice", "hashA", "saltA");
    repo.create("bob", "hashB", "saltB");

    auto alice = repo.findByUsername("alice");
    auto bob = repo.findByUsername("bob");

    assert(alice.has_value() && alice->passwordHash == "hashA");
    assert(bob.has_value() && bob->passwordHash == "hashB");

    std::remove(TEST_DB_PATH);
    log_test("test_two_users_are_independent");
}

void test_duplicate_username_throws() {
    std::remove(TEST_DB_PATH);
    UserRepository repo(TEST_DB_PATH);

    repo.create("alice", "hashA", "saltA");

    bool threw = false;
    try {
        repo.create("alice", "hashB", "saltB");
    } catch (const std::exception&) {
        threw = true;
    }
    assert(threw);

    auto found = repo.findByUsername("alice");
    assert(found->passwordHash == "hashA"); // original record untouched

    std::remove(TEST_DB_PATH);
    log_test("test_duplicate_username_throws");
}

void test_data_persists_across_repository_instances() {
    std::remove(TEST_DB_PATH);
    {
        UserRepository repo(TEST_DB_PATH);
        repo.create("alice", "hashA", "saltA");
    }
    {
        UserRepository repo(TEST_DB_PATH);
        auto found = repo.findByUsername("alice");
        assert(found.has_value());
        assert(found->passwordHash == "hashA");
    }

    std::remove(TEST_DB_PATH);
    log_test("test_data_persists_across_repository_instances");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " STARTING DB UNIT TESTS                 " << std::endl;
    std::cout << "========================================" << std::endl;

    test_generate_salt_produces_different_values();
    test_hash_is_deterministic_for_same_password_and_salt();
    test_hash_differs_with_different_salt();
    test_hash_differs_with_different_password();
    test_create_and_find_round_trip();
    test_find_missing_user_returns_nullopt();
    test_two_users_are_independent();
    test_duplicate_username_throws();
    test_data_persists_across_repository_instances();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL DB TESTS EXECUTED                  " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
