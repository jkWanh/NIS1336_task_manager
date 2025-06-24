#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <string>
#include <filesystem>

class UserManager {
public:
    UserManager();
    bool registerUser(const std::string& username, const std::string& password);
    bool login(const std::string& username, const std::string& password);

private:
    const std::filesystem::path dataDir_ = "data";
    const std::filesystem::path userFile_ = dataDir_ / "users.json";

    void ensureDataDirectoryExists();
};

#endif // USER_H