// HashGenerator.cpp

#include "HashGenerator.h"
#include <windows.h>
#include <wincrypt.h>
#include <sstream>
#include <iomanip>

std::wstring HashGenerator::generateSHA256Hash(const std::string& input) {
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

//std::wstring HashGenerator::generateSHA256Hash(const std::wstring& input) {
//}
