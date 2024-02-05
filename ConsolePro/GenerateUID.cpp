// hash_generator.cpp

#include "GenerateUID.h"

bool UIDGenerator::GenerateAndWriteUID() {
    HKEY hBaseKey = HKEY_LOCAL_MACHINE;
    LPCWSTR subKey = L"Software\\DeviceUIDTest3";

    if (keyExists(hBaseKey, subKey)) {
        std::wcout << L"Verifying UID status" << std::endl;

        // Generate SHA256Hash again
        std::wstring regeneratedUID = generateSHA256Hash("yash");

        // Retrieve the stored UID from the registry
        std::wstring storedUID = readHashFromRegistry();

        // Print both UID values
        //std::wcout << L"Regenerated UID: " << regeneratedUID << std::endl;
        //std::wcout << L"Stored UID: " << storedUID << std::endl;

        // Compare the regenerated UID with the stored UID
        if (regeneratedUID == storedUID) {
            std::wcout << L"UID Matched" << std::endl;
        }
        else {
            std::wcout << L"\n";
            std::wcerr << L"UID Mismatch" << std::endl;
            std::wcout << L"\n";
            std::wcerr << L"Invalid Device" << std::endl;

            exit(1);
        }

        return true;
    }

    // If the key doesn't exist, generate a new UID and write it to the registry
    std::wstring uniqueID = generateSHA256Hash("yash");
    std::wcout << L"Potential New Device" << std::endl;
    std::wcout << L"Generating UID for this Device" << std::endl;
    std::wcout << L"UID Generated" << std::endl;
    return writeHashToRegistry(uniqueID);
}

std::wstring UIDGenerator::generateSHA256Hash(const std::string& input) {
    HCRYPTPROV hProv;
    HCRYPTHASH hHash;

    if (!CryptAcquireContext(&hProv, nullptr, nullptr, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        std::cerr << "Error acquiring cryptographic context" << std::endl;
        return L"";
    }

    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        std::cerr << "Error creating hash object" << std::endl;
        CryptReleaseContext(hProv, 0);
        return L"";
    }

    if (!CryptHashData(hHash, reinterpret_cast<const BYTE*>(input.c_str()), input.length(), 0)) {
        std::cerr << "Error hashing data" << std::endl;
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return L"";
    }

    DWORD hashSize = 0;
    DWORD dataSize = sizeof(DWORD);
    if (!CryptGetHashParam(hHash, HP_HASHSIZE, reinterpret_cast<BYTE*>(&hashSize), &dataSize, 0)) {
        std::cerr << "Error getting hash size" << std::endl;
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return L"";
    }

    std::vector<BYTE> hashData(hashSize, 0);
    if (!CryptGetHashParam(hHash, HP_HASHVAL, hashData.data(), &hashSize, 0)) {
        std::cerr << "Error getting hash value" << std::endl;
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return L"";
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    std::wstringstream ss;
    for (BYTE byte : hashData) {
        ss << std::hex << std::setw(2) << std::setfill(L'0') << static_cast<int>(byte);
    }

    return ss.str();
}



bool UIDGenerator::writeHashToRegistry(const std::wstring& hash) {
    HKEY hKey;
    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, L"Software\\DeviceUIDTest3", 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) {
        std::wcerr << L"Error creating or opening registry key" << std::endl;
        return false;
    }

    if (RegSetValueEx(hKey, L"UniqueID", 0, REG_SZ, reinterpret_cast<const BYTE*>(hash.c_str()), static_cast<DWORD>(hash.size() * sizeof(wchar_t))) != ERROR_SUCCESS) {
        std::wcerr << L"Error writing hash to registry" << std::endl;
        RegCloseKey(hKey);
        return false;
    }

    RegCloseKey(hKey);
    return true;
}

std::wstring UIDGenerator::readHashFromRegistry() {
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\DeviceUIDTest3", 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        std::wcerr << L"Error opening registry key" << std::endl;
        return L"Reg Key opened";
    }

    DWORD dataSize = 0;
    if (RegQueryValueEx(hKey, L"UniqueID", 0, nullptr, nullptr, &dataSize) != ERROR_SUCCESS) {
        std::wcerr << L"Error querying value size" << std::endl;
        RegCloseKey(hKey);
        return L"Value extracted";
    }

    std::vector<BYTE> hashData(dataSize, 0);
    if (RegQueryValueEx(hKey, L"UniqueID", 0, nullptr, hashData.data(), &dataSize) != ERROR_SUCCESS) {
        std::wcerr << L"Error reading hash from registry" << std::endl;
        RegCloseKey(hKey);
        return L"Not able to read hash";
    }

    RegCloseKey(hKey);

    //std::wcout << L"Retrieved Hash Size: " << dataSize << std::endl;
    //std::wcout << L"Retrieved Hash: ";

    //for (BYTE byte : hashData) {
    //    std::wcout << std::hex << std::setw(2) << std::setfill(L'0') << static_cast<int>(byte);
    //}
    //std::wcout << std::endl;

    // Convert the retrieved data to a wide string
    std::wstring storedUID(reinterpret_cast<wchar_t*>(hashData.data()));

    //std::wcout << L"Hash read from registry successfully" << std::endl;
    return storedUID;
}



bool UIDGenerator::keyExists(HKEY hKey, LPCWSTR subKey) {
    HKEY hTestKey;
    LONG result = RegOpenKeyEx(hKey, subKey, 0, KEY_READ, &hTestKey);
    if (result == ERROR_SUCCESS) {
        RegCloseKey(hTestKey);
        return true;
    }
    return false;
}
