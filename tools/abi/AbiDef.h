#pragma once
#include <string>
#include <vector>

namespace platon {
    struct TypdeDef {
        TypdeDef() = default;
        TypdeDef(std::string typeName, std::string realTypeName)
            :typeName(typeName), realTypeName(realTypeName){}

        std::string typeName;
        std::string realTypeName;
    };

    struct ABI {
        std::string methodName;
        std::vector<std::string> args;
        std::vector<TypdeDef> types;
        TypdeDef returnType;
        bool isConst = false;
    };

    struct ABIDef {
        std::vector<ABI> abis;
    };

    struct Event {
        std::string name;
        std::vector<std::string> args;
    };
    struct EventDef {
        std::vector<Event> events;
    };

    struct ContractDef{
        std::string fullName;
        std::string name;
        EventDef eventDef;
    };
}
