#ifndef ANALYSIS_H
#define ANALYSIS_H
#include <unordered_map>
#include <string>
#include <vector>
#include <stdexcept>
#include "../parser/parser.h"
#include "../tokenizer/tokenize.h"

void handleAnalysisError(const std::string& message) {
    std::cerr << "Analysis Error: " << message << std::endl;
    std::exit(1);
}

struct Symbol {
    std::string name;
    std::string type; // E.g., "int", "void"
};

class SymbolTable {
private:
    std::unordered_map<std::string, Symbol> table;

public:
    void addSymbol(const std::string& name, const std::string& type) {
        if (table.count(name)) {
            handleAnalysisError("Symbol already defined: " + name);
        }
        table[name] = {name, type};
    }

    const Symbol& getSymbol(const std::string& name) const {
        if (!table.count(name)) {
            handleAnalysisError("Symbol not found: " + name);
        }
        return table.at(name);
    }
};

void semanticAnalysis(ASTNode* ast, SymbolTable& symbolTable) {
    if (ast->type == "Function") {
        // Add function to the symbol table
        symbolTable.addSymbol(ast->value, "void"); // For now, assume void return type

        // Analyze the function body
        for (auto* child : ast->children) {
            semanticAnalysis(child, symbolTable);
        }
    } else if (ast->type == "ReturnStatement") {
        if (ast->children.empty() || ast->children[0]->type != "Literal" && ast->children[0]->type != "Variable") {
            handleAnalysisError("Invalid return statement");
        }
    }
}

#endif // ANALYSIS_H