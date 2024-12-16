#ifndef ANALYSIS_H
#define ANALYSIS_H
#include <unordered_map>
#include <string>
#include <vector>
#include <stdexcept>
#include "../parser/parser.h"
#include "../tokenizer/tokenize.h"

struct Symbol {
    std::string name;
    std::string type; // E.g., "int", "void"
};
/**
 * @brief Function to handle analysis error
 */
void handleAnalysisError(const std::string& message);

class SymbolTable {
private:
    std::unordered_map<std::string, Symbol> table;

public:
    /**
     * @brief Function to add symbol to the symbol table
     * @param name Name of the symbol
     * @param type Type of the symbol
     */
    void addSymbol(const std::string& name, const std::string& type);

    /**
     * @brief Function to get symbol from the symbol table
     * @param name Name of the symbol
     * @return Symbol
     */
    const Symbol& getSymbol(const std::string& name) const;
};

/**
 * @brief Function to perform semantic analysis
 * @param ast AST node
 * @param symbolTable Symbol table
 */
void semanticAnalysis(ASTNode* ast, SymbolTable& symbolTable);

#endif // ANALYSIS_H