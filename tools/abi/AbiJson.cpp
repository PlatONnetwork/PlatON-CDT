#include <fstream>
#include <regex>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"


#include "AbiDef.h"
#include "AbiJson.h"
#include "Exception.h"
using namespace std;

namespace platon {


    static const string convertBuildinType(const string &typeName) {
        regex charReg(R"(\s*char\s+\*\s*)");
        regex constCharReg(R"(\s*const\s+char\s*\*\s*)");
        string buildInType = typeName;
        if (typeName == "unsigned __int128" || typeName == "uint128_t") buildInType = "uint128";
        else if (typeName == "__int128"          || typeName == "int128_t")  buildInType = "int128";

        else if (typeName == "unsigned long long" || typeName == "uint64_t") buildInType = "uint64";
        else if (typeName == "unsigned long"      || typeName == "uint32_t") buildInType = "uint32";
        else if (typeName == "unsigned short"     || typeName == "uint16_t") buildInType = "uint16";
        else if (typeName == "unsigned char"      || typeName == "uint8_t")  buildInType = "uint8";

        else if (typeName == "long long"          || typeName == "int64_t")  buildInType = "int64";
        else if (typeName == "long"  || typeName == "int32_t" || typeName == "int")  buildInType = "int32";
        else if (typeName == "short"              || typeName == "int16_t")  buildInType = "int16";
        else if (typeName == "char"               || typeName == "int8_t")   buildInType = "int8";
        else if (typeName == "float") buildInType = "float32";
        else if (typeName == "double")   buildInType = "float64";
        else if (regex_search(typeName, charReg) || regex_search(typeName, constCharReg)) buildInType = "string";
        else if (typeName == "void") buildInType = "void";
        else {
            throw Exception() << ErrStr(typeName + " is not buildin type");
        }
        return buildInType;
    }


    void outputJsonABI(const ABIDef &abiDef, const ContractDef &contractDef, std::ofstream &fs) {
        rapidjson::StringBuffer strBuf;
        rapidjson::PrettyWriter <rapidjson::StringBuffer> writer(strBuf);
        //writer.StartObject();
//        writer.Key("version");
//        writer.String("0.01");

        //writer.Key("abi");

        writer.StartArray();
        for (size_t i = 0; i < abiDef.abis.size(); i++) {
            writer.StartObject();
            writer.Key("name");
            writer.String(abiDef.abis[i].methodName);
            writer.Key("inputs");
            writer.StartArray();
            for (size_t j = 0; j < abiDef.abis[i].args.size(); j++) {
                writer.StartObject();
                writer.Key("name");
                writer.String(abiDef.abis[i].args[j]);
//                writer.Key("typeName");
//                writer.String(abiDef.abis[i].types[j].typeName);
                writer.Key("type");
                writer.String(convertBuildinType(abiDef.abis[i].types[j].realTypeName));
                writer.EndObject();
            }
            writer.EndArray();
            writer.Key("outputs");
            writer.StartArray();
            if (abiDef.abis[i].returnType.realTypeName != "void") {
                writer.StartObject();
                writer.Key("name");
                writer.String("");
                writer.Key("type");
                writer.String(convertBuildinType(abiDef.abis[i].returnType.realTypeName));
                writer.EndObject();
            }
            writer.EndArray();
            writer.Key("constant");
            writer.String(abiDef.abis[i].isConst ? "true" : "false");
            writer.Key("type");
            writer.String("function");
            writer.EndObject();
        }
//        writer.EndArray();
//
//
//
//        writer.StartArray();
        for (size_t i = 0; i < contractDef.eventDef.events.size(); i++) {
            writer.StartObject();
            writer.Key("name");
            writer.String(contractDef.eventDef.events[i].name);
            writer.Key("inputs");
            writer.StartArray();
            for (size_t j = 0; j < contractDef.eventDef.events[i].args.size(); j++) {
                writer.StartObject();
                writer.Key("type");
                writer.String(convertBuildinType(contractDef.eventDef.events[i].args[j]));
                writer.EndObject();
            }
            writer.EndArray();
            writer.Key("type");
            writer.String("event");
            writer.EndObject();
        }
        writer.EndArray();

        //writer.EndObject();




        string data = strBuf.GetString();
        fs << data;
        fs.flush();
    }
}
