#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
#include "tokenize.h"

const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"fn", TokenType::KEYWORD},
    {"let", TokenType::KEYWORD},
    {"return", TokenType::KEYWORD},
    {"int", TokenType::KEYWORD}
};

const std::unordered_map<char, TokenType> SYMBOLS = {
    {'=', TokenType::SYMBOL},
    {'+', TokenType::SYMBOL},
    {'-', TokenType::SYMBOL},
    {'*', TokenType::SYMBOL},
    {'/', TokenType::SYMBOL},
    {'(', TokenType::SYMBOL},
    {')', TokenType::SYMBOL},
    {'{', TokenType::SYMBOL},
    {'}', TokenType::SYMBOL},
    {',', TokenType::SYMBOL},
    {':', TokenType::SYMBOL},
    {';', TokenType::SYMBOL}
};


std::vector<Token> tokenize(const std::string& content) {
    std::vector<Token> tokens;
    int line = 1, column = 1;

    for (size_t i = 0; i < content.length(); ++i) {
        char c = content[i];

        if (c == '/' && i + 1 < content.length() && content[i + 1] == '/') {
            // skip until the end of the line or the end of the content
            while (i < content.length() && content[i] != '\n') {
                i++;
            }
            line++;
            column = 1;
            continue;
        }

        // handle whitespace
        if (std::isspace(c)) {
            if (c == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            continue;
        }

        // handle symbols
        if (SYMBOLS.count(c)) {
            tokens.push_back({SYMBOLS.at(c), std::string(1, c), line, column});
            column++;
            continue;
        }

        // handle keywords and identifiers
        if (std::isalpha(c) || c == '_') {
            std::string identifier(1, c);
            while (i + 1 < content.length() && (std::isalnum(content[i + 1]) || content[i + 1] == '_')) {
                identifier += content[++i];
            }

            TokenType type = KEYWORDS.count(identifier) ? KEYWORDS.at(identifier) : TokenType::IDENTIFIER;
            tokens.push_back({type, identifier, line, column});
            column += identifier.length();
            continue;
        }

        // handle numbers
        if (std::isdigit(c)) {
            std::string number(1, c);
            while (i + 1 < content.length() && std::isdigit(content[i + 1])) {
                number += content[++i];
            }
            tokens.push_back({TokenType::NUMBER, number, line, column});
            column += number.length();
            continue;
        }

        // fallback for unknown tokens
        tokens.push_back({TokenType::UNKNOWN, std::string(1, c), line, column});
        column++;
    }

    return tokens;
}

void print_tokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::cout << token.to_string() << std::endl;
    }
}