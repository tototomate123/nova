#include <vector>
#include <string>
#include <stdexcept>
#include "../tokenizer/tokenize.h"
#ifndef PARSER_H
#define PARSER_H

inline void handleError(const std::string& message) {
    std::cerr << "Parsing Error: " << message << std::endl;
    std::exit(1);
}

struct ASTNode {
    std::string type;
    std::string value;
    std::vector<ASTNode*> children;

    ASTNode(const std::string& type, const std::string& value)
        : type(type), value(value), children() {}
};


inline void printAST(const ASTNode* node, int depth = 0) {
    for (int i = 0; i < depth; ++i) std::cout << "  ";
    std::cout << node->type << (node->value.empty() ? "" : ": " + node->value) << std::endl;
    for (const auto& child : node->children) {
        printAST(child, depth + 1);
    }
}

class Parser {
private:
    std::vector<Token> tokens;
    size_t index = 0;

    Token peek() {
        return index < tokens.size() ? tokens[index] : Token{TokenType::UNKNOWN, "", -1, -1};
    }

    Token consume() {
        return index < tokens.size() ? tokens[index++] : Token{TokenType::UNKNOWN, "", -1, -1};
    }

    void expect(TokenType type, const std::string& value = "") {
        Token token = consume();
        if (token.type != type || (!value.empty() && token.value != value)) {
            handleError("Unexpected token: " + token.to_string());
        }
    }

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

    ASTNode* parse() {
        return parseFunction();
    }

private:
    ASTNode* parseFunction() {
    expect(TokenType::KEYWORD, "fn");

    Token name = consume();
    if (name.type != TokenType::IDENTIFIER) {
        handleError("Expected function name, got: " + name.to_string());
    }

    expect(TokenType::SYMBOL, "(");
    expect(TokenType::SYMBOL, ")");
    expect(TokenType::SYMBOL, "{");

    ASTNode* funcNode = new ASTNode("Function", name.value);

    while (peek().type != TokenType::SYMBOL || peek().value != "}") {
        funcNode->children.push_back(parseStatement());
    }

    expect(TokenType::SYMBOL, "}");
    return funcNode;
}

    ASTNode* parseStatement() {
        Token token = peek();

        if (token.type == TokenType::KEYWORD && token.value == "return") {
            return parseReturnStatement();
        }

        handleError("Unknown statement: " + token.to_string());
    }

    ASTNode* parseReturnStatement() {
        expect(TokenType::KEYWORD, "return");

        Token value = consume();
        if (value.type != TokenType::NUMBER && value.type != TokenType::IDENTIFIER) {
            handleError("Expected value after return, got: " + value.to_string());
        }

        expect(TokenType::SYMBOL, ";");

        ASTNode* returnNode = new ASTNode("ReturnStatement", "");
        returnNode->children.push_back(new ASTNode("Literal", value.value));
        return returnNode;
    }
};



#endif