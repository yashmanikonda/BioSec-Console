#ifndef GENERATE_UID_H
#define GENERATE_UID_H

#include <windows.h>
#include <wincrypt.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <codecvt>  // Add this include for std::wstring_convert
#include <windows.h>
#include <devicetopology.h>


//class StringConversion {
//public:
//    static std::string wstringToString(const std::wstring& wstr);
//};

class UIDGenerator {
public:
    static bool GenerateAndWriteUID();

private:
    //static std::wstring generateSHA256Hash(const std::string& input);
    static HRESULT YourFunctionUsingGetDeviceId(IDeviceTopology* pDeviceTopology);
    static std::wstring readHashFromRegistry();
    static bool writeHashToRegistry(const std::wstring& hash);
    static bool keyExists(HKEY hKey, LPCWSTR subKey);
};

#endif // GENERATE_UID_H
