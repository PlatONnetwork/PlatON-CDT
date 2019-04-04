#include <iostream>
#include <regex>

#include "llvm/ADT/STLExtras.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Frontend/FrontendActions.h"
#include "StringUtil.h"
#include "AbiMacro.h"
#include "Log.h"
#include "Common.h"
#include "Exception.h"
using namespace std;
namespace cl = llvm::cl;
using namespace clang;



namespace platon{
    string removeNamespace(string fullName) {
        int i = fullName.size();
        int on_spec = 0;
        int colons = 0;
        while(--i >= 0) {
            if(fullName[i] == '>') {
                ++on_spec; colons=0;
            } else if(fullName[i] == '<') {
                --on_spec; colons=0;
            } else if(fullName[i] == ':' && !on_spec) {
                if (++colons == 2)
                    return fullName.substr(i+2);
            } else {
                colons = 0;
            }
        }
        return fullName;
    }

    void FindMacroAction::ExecuteAction() {
        // getCompilerInstance().getPreprocessor().SetMacroExpansionOnlyInDirectives();
        getCompilerInstance().getPreprocessor().addPPCallbacks(
                llvm::make_unique<MacroCallback>(getCompilerInstance(), *this)
                );
        PreprocessOnlyAction::ExecuteAction();
    }

    void MacroCallback::handleEvent(const Token &token, const MacroDefinition &md, SourceRange range, const MacroArgs *args) {
        const auto& sm = compilerInstance.getSourceManager();
        auto file_name = sm.getFilename(range.getBegin());
        clang::SourceLocation b(range.getBegin()), _e(range.getEnd());
        clang::SourceLocation e(clang::Lexer::getLocForEndOfToken(_e, 0, sm, compilerInstance.getLangOpts()));
        auto macrostr = string(sm.getCharacterData(b), sm.getCharacterData(e)-sm.getCharacterData(b));

        regex r(R"(PLATON_EVENT\s*\(\s*(.+?)\s*,((?:.+?)*)\s*\))");
        smatch smatch;
        auto res = regex_search(macrostr, smatch, r);
        Event event;
        string name = smatch[1];
        trim(name);
        event.name = name;
        vector<string> eventArgs;

        split(smatch[2], eventArgs,",");
        for (size_t i = 0; i < eventArgs.size(); i++) {
            trim(eventArgs[i]);
            if (!isBuildinType(eventArgs[i])) {
                throw Exception() << ErrStr(name +  + ":" + eventArgs[i] + " is not buildin type");
            }
            event.args.push_back(eventArgs[i]);
        }
        act.contractDef.eventDef.events.push_back(event);
    }

    void MacroCallback::MacroExpands (const Token &token, const MacroDefinition &md, SourceRange range, const MacroArgs *args) {
        auto *id = token.getIdentifierInfo();
        if (id == nullptr) return;
        if (id->getName() == "PLATON_EVENT"){
            LOGDEBUG << "get EVENT:" <<  id->getName().str();
            handleEvent(token, md, range, args);
            return;
        }

        if (id->getName() != "PLATON_ABI") return;

        LOGDEBUG << "token name:" << id->getName().str();

        const auto& sm = compilerInstance.getSourceManager();
        auto file_name = sm.getFilename(range.getBegin());
        // if (!act.abi_context.empty() && !file_name.startswith(act.abi_context)) {
        // return;
        // }

        // if (md.getMacroInfo()->getNumArgs() != 2){
        // return;
        // }

        clang::SourceLocation b(range.getBegin()), _e(range.getEnd());
        clang::SourceLocation e(clang::Lexer::getLocForEndOfToken(_e, 0, sm, compilerInstance.getLangOpts()));
        auto macrostr = string(sm.getCharacterData(b), sm.getCharacterData(e)-sm.getCharacterData(b));

        regex r(R"(PLATON_ABI\s*\(\s*(.+?)\s*,((?:.+?)*)\s*\))");
        smatch smatch;
        auto res = regex_search(macrostr, smatch, r);

        act.contractDef.fullName = smatch[1].str();
        act.contractDef.name = removeNamespace(smatch[1].str());
        string action = smatch[2].str();
        trim(action);
        act.actions.push_back(action);
        LOGDEBUG << "macrostr:" << macrostr
            << "  contract:" << smatch[1].str()
            << "  actions_str:" << action << endl;

        // MacroInfo *info = md.getMacroInfo();
        // if (info != nullptr) {
        // vector<const IdentifierInfo *> ids = info->params().vec();
        // for (int i = 0; i < ids.size(); i++) {
        // cout << "IdentifierInfo:" << ids[i]->getName().str() << endl;
        // }

        // }

    }
}
