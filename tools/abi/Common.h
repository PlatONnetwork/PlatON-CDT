//
// Created by zhou.yang on 2018/10/20.
//

#pragma once

#include <string>
#include <regex>

namespace platon {

    inline bool isBuildinType(const std::string &typeName) {
        std::regex charArrayReg(R"(char\s*\[\d+\])");
        std::regex charReg(R"(\s*char\s+\*\s*)");
        std::regex constCharReg(R"(\s*const\s+char\s*\*\s*)");
        //std::smatch smatch;
        if (typeName == "unsigned __int128" || typeName == "uint128_t"
            || typeName == "__int128" || typeName == "int128_t"
            || typeName == "unsigned long long" || typeName == "uint64_t"
            || typeName == "unsigned long" || typeName == "uint32_t"
            || typeName == "unsigned short" || typeName == "uint16_t"
            || typeName == "unsigned char" || typeName == "uint8_t"
            || typeName == "long long" || typeName == "int64_t"
            || typeName == "long" || typeName == "int32_t"
            || typeName == "short" || typeName == "int16_t"
            || typeName == "char" || typeName == "int8_t"
            || typeName == "int" || typeName == "double"
            ||  std::regex_search(typeName, charReg) || std::regex_search(typeName, constCharReg) || std::regex_search(typeName, charArrayReg)){
            return true;
        }
        return false;
    }
}