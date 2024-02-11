// hash_generator.cpp

#include "GenerateUID.h"
#include "HashGenerator.h"
#include <comdef.h>
#include <Wbemidl.h>
#include <Windows.h>
#include <Mmdeviceapi.h>

#include <iostream> // Include for print statements

#pragma comment(lib, "wbemuuid.lib")

// Function to retrieve the product ID using WMI
std::wstring GetProductID() {
    CoInitializeEx(0, COINIT_MULTITHREADED);

    IWbemLocator* pLoc = nullptr;
    CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&pLoc);

    IWbemServices* pSvc = nullptr;
    pLoc->ConnectServer(bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, nullptr, 0, nullptr, nullptr, &pSvc);
    pLoc->Release();

    CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);

    IEnumWbemClassObject* pEnumerator = nullptr;
    pSvc->ExecQuery(_bstr_t("WQL"), _bstr_t("SELECT * FROM Win32_OperatingSystem"), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &pEnumerator);

    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;

    std::wstring productID;

    while (pEnumerator) {
        pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (uReturn == 0) {
            break;
        }

        VARIANT vtProp;
        pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
        productID = vtProp.bstrVal;
        VariantClear(&vtProp);

        pclsObj->Release();
    }

    pSvc->Release();
    CoUninitialize();

    return productID;
}

bool UIDGenerator::GenerateAndWriteUID() {
    HKEY hBaseKey = HKEY_LOCAL_MACHINE;
    LPCWSTR subKey = L"Software\\DeviceUIDTest4";
    
    std::wstring productID = GetProductID();

    std::string productIDString(productID.begin(), productID.end());

    if (keyExists(hBaseKey, subKey)) {
        std::wcout << L"Verifying UID status" << std::endl;

        // Generate SHA256Hash again
        std::wstring regeneratedUID = HashGenerator::generateSHA256Hash(productIDString);

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
    std::wstring uniqueID = HashGenerator::generateSHA256Hash(productIDString);
    std::wcout << L"Potential New Device" << std::endl;
    std::wcout << L"Generating UID for this Device" << std::endl;
    std::wcout << L"UID Generated" << std::endl;
    return writeHashToRegistry(uniqueID);
}

bool UIDGenerator::writeHashToRegistry(const std::wstring& hash) {
    HKEY hKey;
    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, L"Software\\DeviceUIDTest4", 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) {
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
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\DeviceUIDTest4", 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
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
