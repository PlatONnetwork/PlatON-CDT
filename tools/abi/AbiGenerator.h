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


namespace platon {

    class ABIGenerator {
        private:
            clang::CompilerInstance *compilerInstance;

            const std::string &contract;
            const std::vector<std::string> &actions;
            ABIDef &abiDef;
        public:
            ABIGenerator(const std::string &contract, const std::vector<std::string> &actions, ABIDef &abiDef)
            :contract(contract), actions(actions), abiDef(abiDef){}

            ~ABIGenerator(){}

            void setCompilerInstance(clang::CompilerInstance &compilerInstance){
                this->compilerInstance = &compilerInstance;
            }
            void handleTagDeclDefinition(clang::TagDecl* tagDecl);
            void getRealName( clang::QualType &type, clang::ASTContext* astContext, std::string &typeName, std::string &realTypeName);
    };

    class ABIAstConsumer : public clang::ASTConsumer {
        public:
            clang::CompilerInstance &compilerInstance;
            ABIGenerator& abiGen;

            ABIAstConsumer(clang::CompilerInstance& compilerInstance, ABIGenerator& abiGen)
                :compilerInstance(compilerInstance), abiGen(abiGen) {
                abiGen.setCompilerInstance(compilerInstance);
            }

            void HandleTagDeclDefinition(clang::TagDecl* tagDecl) override {
                abiGen.handleTagDeclDefinition(tagDecl);
            }
    };

    class ABIAction : public clang::ASTFrontendAction {

        private:
            const std::string &contract;
            const std::vector<std::string> &actions;
            ABIDef &abiDef;
            ABIGenerator abiGen;

        public:
            ABIAction(const std::string& contract, const std::vector<std::string>& actions, ABIDef &abiDef)
                :contract(contract), actions(actions), abiDef(abiDef), abiGen(contract, actions, abiDef){}

        protected:
            std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& compilerInstance,
                    llvm::StringRef) override {
                return llvm::make_unique<ABIAstConsumer>(compilerInstance, abiGen);
            }
    };
}
