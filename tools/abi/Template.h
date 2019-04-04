#pragma once

#include "AbiDef.h"
#include <string>
namespace platon {

    std::string generateAbiCPlusPlus(ContractDef &contractDef, ABIDef &abiDef);
}