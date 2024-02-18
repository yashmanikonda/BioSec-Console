// PasswordHandling.cpp
#include "PasswordHandling.h"
#include "HashGenerator.h"
#include "GenerateUID.h" // Include your existing UIDGenerator class header
#include <bcrypt.h>      // Include the bcrypt library
#include <iostream>
#include <Windows.h>

PasswordHandler::PasswordHandler() {
    // Initialize any necessary configurations or resources
}

PasswordHandler::~PasswordHandler() {
    // Clean up any resources if needed
}

bool PasswordHandler::checkPassword(const std::string& inputPassword) {
    std::string storedHash = readHashFromRegistry();
    if (storedHash.empty()) {
        // No stored hash, authentication fails
        std::cerr << "Password not set. Please set a password first." << std::endl;
        return false;
    }

    // Hash the input password and compare with the stored hash
    std::string inputHash = HashGenerator::generateSHA256HashS(inputPassword);

    std::cout << "Input Hash: " << inputHash << std::endl;
    std::cout << "Stored Hash: " << storedHash << std::endl;

    // Check if the input hash matches the stored hash
    if (inputHash == storedHash) {
        // Password is correct
        std::cout << "Password verified\n";
        return true;
    }
    else {
        // Password is incorrect
        std::cerr << "Authentication failed. Incorrect password." << std::endl;
        return false;
    }
}

bool PasswordHandler::setNewPassword(const std::string& newPassword, const std::string& confirmNewPassword) {
    // Check if the registry key exists
    HKEY hBaseKey = HKEY_LOCAL_MACHINE;
    LPCWSTR subKey = L"Software\\PBioword";

    if (!keyExists(hBaseKey, subKey)) {
        std::cout << "Creating a new password. Please confirm your password: ";

        if (newPassword == confirmNewPassword) {
            // Set and store the new password hash
            std::string newHash = HashGenerator::generateSHA256HashS(newPassword);
            return writeHashToRegistry(newHash);
        }
        else {
            std::cerr << "Passwords do not match. Unable to create a new password." << std::endl;
            return false;
        }
    }
    else {
        std::cerr << "Cannot create a new password. Password already exists." << std::endl;
        return false;
    }
}

std::string PasswordHandler::readHashFromRegistry() {
    HKEY hKey;
    LPCWSTR subKey = L"Software\\PBioword";
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return ""; // Registry key not found
    }

    DWORD dataSize = 0;
    if (RegQueryValueEx(hKey, L"PasswordHash", 0, nullptr, nullptr, &dataSize) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return ""; // Error querying value size or value not found
    }

    std::vector<BYTE> hashData(dataSize, 0);
    if (RegQueryValueEx(hKey, L"PasswordHash", 0, nullptr, hashData.data(), &dataSize) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return ""; // Error reading hash from registry
    }

    RegCloseKey(hKey);

    // Convert the retrieved data to a string
    return std::string(reinterpret_cast<char*>(hashData.data()), dataSize);
}

bool PasswordHandler::writeHashToRegistry(const std::string& hash) {
    HKEY hKey;
    LPCWSTR subKey = L"Software\\PBioword";
    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, subKey, 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) {
        std::cerr << "Error creating or opening registry key" << std::endl;
        return false;
    }

    // Clear existing registry value
    RegDeleteValue(hKey, L"PasswordHash");

    if (RegSetValueEx(hKey, L"PasswordHash", 0, REG_SZ, reinterpret_cast<const BYTE*>(hash.c_str()), static_cast<DWORD>(hash.size())) != ERROR_SUCCESS) {
        std::cerr << "Error writing hash to registry" << std::endl;
        RegCloseKey(hKey);
        return false;
    }

    RegCloseKey(hKey);
    return true;
}

bool PasswordHandler::keyExists(HKEY hKey, LPCWSTR subKey) {
    HKEY hTestKey;
    LONG result = RegOpenKeyEx(hKey, subKey, 0, KEY_READ, &hTestKey);
    if (result == ERROR_SUCCESS) {
        RegCloseKey(hTestKey);
        return true;
    }
    return false;
}

int PasswordHandler::HandlePasswordLogic() {
    std::cout << "Password handling logic...\n";

    HKEY hBaseKey = HKEY_LOCAL_MACHINE;
    LPCWSTR subKey = L"Software\\PBioword";

    if (keyExists(hBaseKey, subKey)) {
        std::cout << "Password exists. Verify your password\n";
        std::string inputPassword;
        std::cout << "Enter your password: ";
        std::cin >> inputPassword;

        if (checkPassword(inputPassword)) {
            std::cout << "Password verified\n";
        }
        else {
            std::cerr << "Authentication failed. Incorrect password.\n";
            return 1; // Return an error code or handle accordingly
        }
    }
    else {
        std::cout << "Creating a new password...\n";
        std::string newPassword;

        std::cout << "Enter your new password: ";
        std::cin >> newPassword;

        std::cout << "Confirm your new password: ";
        std::string confirmNewPassword;
        std::cin >> confirmNewPassword;

        if (setNewPassword(newPassword, confirmNewPassword)) {
            std::cout << "Password set successfully\n";
        }
        else {
            std::cerr << "Error setting or verifying password\n";
            return 1; // Return an error code or handle accordingly
        }
    }

    return 0;
}
