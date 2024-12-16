#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <iostream>

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    SYMBOL,
    WHITESPACE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    std::string to_string() const {
        std::string type_str;
        switch (type) {
            case TokenType::KEYWORD: type_str = "KEYWORD"; break;
            case TokenType::IDENTIFIER: type_str = "IDENTIFIER"; break;
            case TokenType::NUMBER: type_str = "NUMBER"; break;
            case TokenType::SYMBOL: type_str = "SYMBOL"; break;
            case TokenType::WHITESPACE: type_str = "WHITESPACE"; break;
            case TokenType::UNKNOWN: type_str = "UNKNOWN"; break;
        }
        return "Token(" + type_str + ", \"" + value + "\", " +
               std::to_string(line) + ", " + std::to_string(column) + ")";
    }
};

/**
 * @brief Function to tokenize the content
 * @param content Content to tokenize
 * @return Vector of tokens
 */
std::vector<Token> tokenize(const std::string& content);

/**
 * @brief Function to print tokens
 * @param tokens Vector of tokens
 */
void print_tokens(const std::vector<Token>& tokens);

#endif // TOKENIZE_H
