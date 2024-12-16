#include "analysis.h"

void handleAnalysisError(const std::string& message) {
    std::cerr << "Analysis Error: " << message << std::endl;
    std::exit(1);
}

void SymbolTable::addSymbol(const std::string& name, const std::string& type) {
    if (table.count(name)) {
        handleAnalysisError("Symbol already defined: " + name);
    }
    table[name] = {name, type};
}

const Symbol& SymbolTable::getSymbol(const std::string& name) const {
    if (!table.count(name)) {
        handleAnalysisError("Symbol not found: " + name);
    }
    return table.at(name);
}

void semanticAnalysis(ASTNode* ast, SymbolTable& symbolTable) {
    if (ast->type == "Function") {
        // Add function to symbol table
        symbolTable.addSymbol(ast->value, "void"); // For now, assume void return type

        // Analyze function body
        for (auto* child : ast->children) {
            semanticAnalysis(child, symbolTable);
        }
    } else if (ast->type == "ReturnStatement") {
        if (ast->children.empty() || (ast->children[0]->type != "Literal" && ast->children[0]->type != "Variable")) {
            handleAnalysisError("Invalid return statement");
        }
    }
}
