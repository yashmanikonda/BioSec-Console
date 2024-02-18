#pragma once

// HashGenerator.h

#ifndef HASH_GENERATOR_H
#define HASH_GENERATOR_H

#include <windows.h>
#include <wincrypt.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <codecvt> 

class HashGenerator {
public:
    // for Generate UID
    static std::wstring generateSHA256Hash(const std::string& input);
    static std::string generateSHA256HashS(const std::string& input);
    static std::wstring generateSHA256HashW(const std::wstring& input);
};

#endif // HASH_GENERATOR_H
