#include "UserManager.h"
#include <sodium.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

UserManager::UserManager() {
    if (sodium_init() < 0) {
        throw std::runtime_error("Failed to initialize libsodium");
    }
    ensureDataDirectoryExists();
}

void UserManager::ensureDataDirectoryExists() {
    if (!std::filesystem::exists(dataDir_)) {
        std::filesystem::create_directory(dataDir_);
    }
}

bool UserManager::registerUser(const std::string& username, const std::string& password) {
    json users;
    if (std::filesystem::exists(userFile_)) {
        std::ifstream i(userFile_);
        if (i.peek() != std::ifstream::traits_type::eof()) {
            i >> users;
        }
    }

    if (users.contains(username)) {
        std::cerr << "Error: User '" << username << "' already exists." << std::endl;
        return false;
    }

    char hashed_password[crypto_pwhash_STRBYTES];
    if (crypto_pwhash_str(hashed_password, password.c_str(), password.length(),
                          crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        std::cerr << "Error: Failed to hash password." << std::endl;
        return false;
    }

    users[username] = hashed_password;

    std::ofstream o(userFile_);
    o << users.dump(4);
    std::cout << "User '" << username << "' registered successfully." << std::endl;
    return true;
}

bool UserManager::login(const std::string& username, const std::string& password) {
    if (!std::filesystem::exists(userFile_)) {
        std::cerr << "Error: No users registered." << std::endl;
        return false;
    }

    std::ifstream i(userFile_);
    json users;
    if (i.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "Error: No users registered." << std::endl;
        return false;
    }
    i >> users;

    if (!users.contains(username)) {
        std::cerr << "Error: User '" << username << "' not found." << std::endl;
        return false;
    }

    std::string stored_hash = users[username];
    if (crypto_pwhash_str_verify(stored_hash.c_str(), password.c_str(), password.length()) != 0) {
        std::cerr << "Error: Invalid password." << std::endl;
        return false;
    }

    return true;
}