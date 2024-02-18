// PasswordHandling.h
#pragma once

#include <string>
#include <Windows.h>


class PasswordHandler {
public:
    PasswordHandler();
    ~PasswordHandler();
    int HandlePasswordLogic();

private:
    // Function to check if a password matches the stored hash
    bool checkPassword(const std::string& inputPassword);

    //bool changePassword();

    // Function to set and store a new password hash
    bool setNewPassword(const std::string& newPassword, const std::string& confirmNewPassword);

    // Add any private members or functions as needed
    std::string readHashFromRegistry();
    bool writeHashToRegistry(const std::string& hash);
    bool keyExists(HKEY hKey, LPCWSTR subKey);
};
