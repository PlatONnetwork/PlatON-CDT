#pragma once
#include "boost/filesystem.hpp"

#include "AbiDef.h"

namespace platon {
    void outputJsonABI(const ABIDef &abiDef, const ContractDef &contractDef, std::ofstream &fs);
}

