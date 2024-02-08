//#include "PasswordHandling.h"
//#include "GenerateUID.h" // Include your existing UIDGenerator class header
//#include <bcrypt.h>      // Include the bcrypt library
//
//PasswordHandler::PasswordHandler() {
//    // Initialize any necessary configurations or resources
//}
//
//PasswordHandler::~PasswordHandler() {
//    // Clean up any resources if needed
//}
//
//bool PasswordHandler::checkPassword(const std::string& password) {
//    std::string storedHash = readHashFromRegistry();
//    if (storedHash.empty()) {
//        // No stored hash, authentication fails
//        return false;
//    }
//
//    // Hash the input password and compare with the stored hash
//    std::string inputHash = UIDGenerator::generateSHA256Hash(password);
//    return inputHash == storedHash;
//}
//
//bool PasswordHandler::setNewPassword(const std::string& password) {
//    // Check if the registry key exists
//    HKEY hBaseKey = HKEY_LOCAL_MACHINE;
//    LPCWSTR subKey = L"Software\\PBioword";
//
//    if (keyExists(hBaseKey, subKey)) {
//        std::wcout << L"Enter your current password: ";
//        std::wstring currentPassword;
//        std::wcin >> currentPassword;
//
//        // Check if the current password matches the stored hash
//        std::string storedHash = readHashFromRegistry();
//        std::string inputHash = UIDGenerator::generateSHA256Hash(StringConversion::wstringToString(currentPassword));
//        if (inputHash != storedHash) {
//            std::wcerr << L"Incorrect current password. Unable to change the password." << std::endl;
//            return false;
//        }
//    }
//    else {
//        std::wcout << L"Creating a new password. Please confirm your password: ";
//        std::wstring confirmPassword;
//        std::wcin >> confirmPassword;
//
//        if (password != StringConversion::wstringToString(confirmPassword)) {
//            std::wcerr << L"Passwords do not match. Unable to create a new password." << std::endl;
//            return false;
//        }
//    }
//
//    // Set and store the new password hash
//    std::string newHash = UIDGenerator::generateSHA256Hash(password);
//    return writeHashToRegistry(newHash);
//}
//
//std::string PasswordHandler::readHashFromRegistry() {
//    HKEY hKey;
//    LPCWSTR subKey = L"Software\\PBioword";
//    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
//        return ""; // Registry key not found
//    }
//
//    DWORD dataSize = 0;
//    if (RegQueryValueEx(hKey, L"PasswordHash", 0, nullptr, nullptr, &dataSize) != ERROR_SUCCESS) {
//        RegCloseKey(hKey);
//        return ""; // Error querying value size or value not found
//    }
//
//    std::vector<BYTE> hashData(dataSize, 0);
//    if (RegQueryValueEx(hKey, L"PasswordHash", 0, nullptr, hashData.data(), &dataSize) != ERROR_SUCCESS) {
//        RegCloseKey(hKey);
//        return ""; // Error reading hash from registry
//    }
//
//    RegCloseKey(hKey);
//
//    // Convert the retrieved data to a string
//    return std::string(reinterpret_cast<char*>(hashData.data()), hashData.size());
//}
//
//bool PasswordHandler::writeHashToRegistry(const std::string& hash) {
//    HKEY hKey;
//    LPCWSTR subKey = L"Software\\PBioword";
//    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, subKey, 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) {
//        std::wcerr << L"Error creating or opening registry key" << std::endl;
//        return false;
//    }
//
//    if (RegSetValueEx(hKey, L"PasswordHash", 0, REG_SZ, reinterpret_cast<const BYTE*>(hash.c_str()), static_cast<DWORD>(hash.size())) != ERROR_SUCCESS) {
//        std::wcerr << L"Error writing hash to registry" << std::endl;
//        RegCloseKey(hKey);
//        return false;
//    }
//
//    RegCloseKey(hKey);
//    return true;
//}
//
//bool PasswordHandler::keyExists(HKEY hKey, LPCWSTR subKey) {
//    HKEY hTestKey;
//    LONG result = RegOpenKeyEx(hKey, subKey, 0, KEY_READ, &hTestKey);
//    if (result == ERROR_SUCCESS) {
//        RegCloseKey(hTestKey);
//        return true;
//    }
//    return false;
//}
