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
        // Analyze function body
        for (auto* child : ast->children) {
            semanticAnalysis(child, symbolTable);
        }
    } else if (ast->type == "VariableDeclaration") {
        std::string varName = ast->value;
        std::string inferredType;

        // Check if type is explicitly declared
        if (ast->children[0]->type == "Type") {
            inferredType = ast->children[0]->value;
        } else {
            // Infer type from the assigned value
            ASTNode* valueNode = ast->children[0];
            if (valueNode->type == "Literal") {
                if (std::all_of(valueNode->value.begin(), valueNode->value.end(), ::isdigit)) {
                    inferredType = "int";
                } else {
                    handleAnalysisError("Unable to infer type for: " + varName);
                }
            } else if (valueNode->type == "Variable") {
                Symbol symbol = symbolTable.getSymbol(valueNode->value);
                inferredType = symbol.type;
            } else {
                handleAnalysisError("Unable to infer type for: " + varName);
            }
        }

        // Add the variable to the symbol table
        symbolTable.addSymbol(varName, inferredType);
        log::debug("Added variable: " + varName + " with type: " + inferredType);
    } else if (ast->type == "ReturnStatement") {
        if (ast->children.empty() || 
            (ast->children[0]->type != "Literal" && ast->children[0]->type != "Variable")) {
            handleAnalysisError("Invalid return statement");
        }
    }
}