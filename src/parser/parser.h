#include <vector>
#include <string>
#include <stdexcept>
#include "../tokenizer/tokenize.h"
#include "../logger/logger.h"
#ifndef PARSER_H
#define PARSER_H

struct ASTNode {
    std::string type;
    std::string value;
    std::vector<ASTNode*> children;

    ASTNode(const std::string& type, const std::string& value)
        : type(type), value(value), children() {}
};

/**
 * @brief Function to handle parsing error
 */
void handleError(const std::string& message);
/**
 * @brief Function to print AST
 * @param node AST node
 */
void printAST(const ASTNode* node, int depth = 0);

class Parser {
private:
    std::vector<Token> tokens;
    size_t index = 0;
    /**
     * @brief Function to peek the next token
     * @return Token
     */
    Token peek();
    /**
     * @brief Function to consume the next token
     * @return Token
     */
    Token consume();
    /**
     * @brief Function to expect a token
     * @param type Token type
     * @param value Token value
     * @throws std::exit(1) if token is not as expected
     */
    void expect(TokenType type, const std::string& value = "");

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {}
    /**
     * @brief Function to parse the tokens
     * @return ASTNode
     */
    ASTNode* parse();

private:
    /**
     * @brief Function to parse a function
     * @return ASTNode
     */
    ASTNode* parseFunction();

    /**
     * @brief Function to parse a statement
     * @return ASTNode
     */
    ASTNode* parseStatement();

    /**
     * @brief Function to parse a block
     * @return ASTNode
     */
    ASTNode* parseVariableDeclaration();

    /**
     * @brief Function to parse an expression
     * @return ASTNode
     */
    ASTNode* parseExpression();

    /**
     * @brief Function to parse a return statement
     * @return ASTNode
     */
    ASTNode* parseReturnStatement();
};

#endif