#include "parser.h"

void handleError(const std::string& message) {
    std::cerr << "Parsing Error: " << message << std::endl;
    std::exit(1);
}

void printAST(const ASTNode* node, int depth) {
    for (int i = 0; i < depth; ++i) std::cout << "  ";
    std::cout << node->type << (node->value.empty() ? "" : ": " + node->value) << std::endl;
    for (const auto& child : node->children) {
        printAST(child, depth + 1);
    }
}



Token Parser::peek() {
    return index < tokens.size() ? tokens[index] : Token{TokenType::UNKNOWN, "", -1, -1};
}

Token Parser::consume() {
    return index < tokens.size() ? tokens[index++] : Token{TokenType::UNKNOWN, "", -1, -1};
}

void Parser::expect(TokenType type, const std::string& value) {
    Token token = consume();
    if (token.type != type || (!value.empty() && token.value != value)) {
        handleError("Unexpected token: " + token.to_string());
    }
}

ASTNode* Parser::parse() {
    return parseFunction();
}

ASTNode* Parser::parseFunction() {
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

ASTNode* Parser::parseStatement() {
    Token token = peek();

    if (token.type == TokenType::KEYWORD && token.value == "return") {
        return parseReturnStatement();
    }

    if (token.type == TokenType::KEYWORD && token.value == "let") {
        return parseVariableDeclaration();
    }

    handleError("Unknown statement: " + token.to_string());
    return nullptr;
}

ASTNode* Parser::parseVariableDeclaration() {
    log::debug("Parsing variable declaration");
    expect(TokenType::KEYWORD, "let");

    Token name = consume();
    if (name.type != TokenType::IDENTIFIER) {
        handleError("Expected variable name, got: " + name.to_string());
    }

    // Optional type declaration
    Token next = peek();
    ASTNode* typeNode = nullptr;
    if (next.type == TokenType::SYMBOL && next.value == ":") {
        consume(); // Consume ':'
        Token type = consume();
        if (type.type != TokenType::KEYWORD) {
            handleError("Expected type, got: " + type.to_string());
        }
        typeNode = new ASTNode("Type", type.value);
    }

    expect(TokenType::SYMBOL, "=");
    log::debug("Parsing expression");
    ASTNode* value = parseExpression();

    expect(TokenType::SYMBOL, ";"); // Ensure semicolon at the end

    ASTNode* varNode = new ASTNode("VariableDeclaration", name.value);
    if (typeNode) {
        varNode->children.push_back(typeNode); // Add type if declared
    }
    varNode->children.push_back(value);
    return varNode;
}



int getPrecedence(const std::string& op) {
    if (op == "+" || op == "-") return 1; // Lower precedence
    if (op == "*" || op == "/") return 2; // Higher precedence
    return 0; // Invalid operator
}

ASTNode* Parser::parsePrimary() {
    Token token = peek();

    // Handle parentheses
    if (token.type == TokenType::SYMBOL && token.value == "(") {
        consume(); // Consume '('
        ASTNode* expr = parseExpression(); // Parse the inner expression
        expect(TokenType::SYMBOL, ")"); // Ensure closing ')'
        return expr;
    }

    // Handle literals and variables
    Token lhs = consume();
    if (lhs.type == TokenType::IDENTIFIER || lhs.type == TokenType::NUMBER) {
        return new ASTNode(lhs.type == TokenType::IDENTIFIER ? "Variable" : "Literal", lhs.value);
    }

    handleError("Expected identifier, number, or parenthesis, got: " + lhs.to_string());
    return nullptr;
}


ASTNode* Parser::parseBinaryOpRHS(int exprPrecedence, ASTNode* left) {
    while (true) {
        Token op = peek();

        // Stop parsing if the token is not an operator or indicates the end of an expression
        if (op.type == TokenType::SYMBOL && op.value == ";") return left; // End of statement
        if (op.type == TokenType::KEYWORD && op.value == "let") return left; // Start of a new statement
        if (op.type != TokenType::SYMBOL || getPrecedence(op.value) == 0) return left; // Non-operator

        int tokenPrecedence = getPrecedence(op.value);

        // Stop parsing if the current operator has lower precedence
        if (tokenPrecedence < exprPrecedence) {
            return left;
        }

        consume(); // Consume the operator

        // Parse the next primary expression
        ASTNode* right = parsePrimary();

        // Check if the next operator has higher precedence
        Token nextOp = peek();
        int nextPrecedence = getPrecedence(nextOp.value);
        if (tokenPrecedence < nextPrecedence) {
            right = parseBinaryOpRHS(tokenPrecedence + 1, right);
        }

        // Create a new BinaryOp node
        ASTNode* opNode = new ASTNode("BinaryOp", op.value);
        opNode->children.push_back(left);
        opNode->children.push_back(right);
        left = opNode;
    }
}

ASTNode* Parser::parseExpression() {
    ASTNode* left = parsePrimary(); // Parse the left-hand side
    return parseBinaryOpRHS(0, left); // Process the rest based on precedence
}



ASTNode* Parser::parseReturnStatement() {
    expect(TokenType::KEYWORD, "return");

    Token value = consume();
    if (value.type != TokenType::NUMBER && value.type != TokenType::IDENTIFIER) {
        handleError("Expected value after return, got: " + value.to_string());
    }

    expect(TokenType::SYMBOL, ";");

    ASTNode* returnNode = new ASTNode("ReturnStatement", "");
    if (value.type == TokenType::NUMBER) {
        returnNode->children.push_back(new ASTNode("Literal", value.value));
    } else if (value.type == TokenType::IDENTIFIER) {
        returnNode->children.push_back(new ASTNode("Variable", value.value));
    }
    return returnNode;
}