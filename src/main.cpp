#include <iostream>
#include <string>
#include <fstream>
#include "logger/logger.h"
#include "tokenizer/tokenize.h"
#include "parser/parser.h"
#include "analysis/analysis.h"
#include "llvm/llvm_generator.h"

void read_file(const std::string &filename, std::string &content);

int main(int argc, char** argv) {

    if (argc < 2) {
        log::error("No input file provided as argument");
        return 1;
    }

    log::debug("Trying to read file stream");
    std::string content = "";
    read_file(argv[1], content);
    log::debug("File content:\n" + content);
    log::debug("Tokenizing file");
    auto tokens = tokenize(content);
    print_tokens(tokens);
    log::debug("AST:");

    Parser parser(tokens);
    ASTNode* ast = parser.parse();
    if (!ast) {
        log::error("Failed to parse the input.");
        return 1;
    }
    printAST(ast);

    SymbolTable symbolTable;
    semanticAnalysis(ast, symbolTable);
    log::debug("Semantic analysis completed");
    initializeLLVM();
    generateLLVMIR(ast);
    log::debug("LLVM IR generated");

    printLLVMIR("output.ll");

    delete ast;
    log::info("Compilation successful");
    return 0;
}



void read_file(const std::string &filename, std::string &content) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        log::error("Failed to open file: " + filename);
        return;
    }
    log::debug("Opened file: " + filename);
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    file.close();

    log::debug("Closed file: " + filename);
}