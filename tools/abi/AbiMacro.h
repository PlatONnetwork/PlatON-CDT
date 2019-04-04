#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "clang/Driver/Options.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Lex/Preprocessor.h"

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Sema/Sema.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/MacroArgs.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/AST/QualTypeNames.h"

#include "AbiDef.h"
namespace cl = llvm::cl;

namespace platon{

    struct FindMacroAction : public clang::PreprocessOnlyAction {
        ContractDef& contractDef;
        std::vector<std::string>& actions;

        FindMacroAction(ContractDef& contractDef, std::vector<std::string>& actions)
            :contractDef(contractDef),
            actions(actions){}

        void ExecuteAction() override;
    };

    struct MacroCallback : public clang::PPCallbacks {

        clang::CompilerInstance& compilerInstance;
        FindMacroAction &act;

        MacroCallback(clang::CompilerInstance& compilerInstance, FindMacroAction& act)
            : compilerInstance(compilerInstance), act(act) {
            }
        void handleEvent(const clang::Token &token, const clang::MacroDefinition &md, clang::SourceRange range, const clang::MacroArgs *args);
        void MacroExpands (const clang::Token &token, const clang::MacroDefinition &md, clang::SourceRange range, const clang::MacroArgs *args) override ;
    };

}
