#include "llvm_generator.h"
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Verifier.h>
#include <fstream>
#include "llvm_generator.h"
#include "../logger/logger.h"
#include <map>

llvm::LLVMContext context;
llvm::IRBuilder<> builder(context);
llvm::Module* module = nullptr;


llvm::Value* generateExpression(ASTNode* ast, std::map<std::string, llvm::Value*> &variableMap);

void initializeLLVM() {
    log::debug("Initializing LLVM");
    module = new llvm::Module("cts_module", context);
}

void generateLLVMIR(ASTNode* ast) {
    if (!ast) {
        log::error("AST is null");
        return;
    }

    log::debug("Generating LLVM IR for node type: " + ast->type);

    if (ast->type == "Function") {
        log::debug("Defining function: " + ast->value);

        llvm::FunctionType* funcType = llvm::FunctionType::get(builder.getInt32Ty(), false);
        llvm::Function* function = llvm::Function::Create(
            funcType, llvm::Function::ExternalLinkage, ast->value, module);

        llvm::BasicBlock* block = llvm::BasicBlock::Create(context, "entry", function);
        builder.SetInsertPoint(block);

        std::map<std::string, llvm::Value*> variableMap;

        for (auto* child : ast->children) {
            if (child->type == "VariableDeclaration") {
                std::string varName = child->value;
                llvm::Value* initValue = generateExpression(child->children[1], variableMap);
                llvm::AllocaInst* alloca = builder.CreateAlloca(builder.getInt32Ty(), nullptr, varName);
                builder.CreateStore(initValue, alloca);
                variableMap[varName] = alloca;
                log::debug("Declared variable: " + varName);
            } else if (child->type == "ReturnStatement") {
                llvm::Value* retValue = generateExpression(child->children[0], variableMap);
                if (!retValue) {
                    log::error("Failed to generate return value");
                    return;
                }
                builder.CreateRet(retValue);
                log::debug("Added return value");
            }
        }
        if (!function->back().getTerminator()) {
            builder.CreateRet(builder.getInt32(0));
            log::debug("Added default return value for function: " + ast->value);
        }

        std::string errorMsg;
        llvm::raw_string_ostream errorStream(errorMsg);
        if (llvm::verifyFunction(*function, &errorStream)) {
            log::error("LLVM function verification failed for: " + ast->value);
            log::error("Verification error: " + errorStream.str());
            return;
        }

        log::debug("Function verified successfully: " + ast->value);
    } else {
        log::debug("Unhandled AST node type: " + ast->type);
    }
}



llvm::Value* generateExpression(ASTNode* ast, std::map<std::string, llvm::Value*>& variableMap) {
    if (ast->type == "Literal") {
        if (!std::all_of(ast->value.begin(), ast->value.end(), ::isdigit)) {
            handleError("Invalid Literal value: " + ast->value);
        }
        log::debug("Converting Literal to integer: " + ast->value);
        return llvm::ConstantInt::get(builder.getInt32Ty(), std::stoi(ast->value));
    } else if (ast->type == "Variable") {
        if (variableMap.count(ast->value) == 0) {
            handleError("Variable not found: " + ast->value);
        }
        log::debug("Loading variable: " + ast->value);
        return builder.CreateLoad(builder.getInt32Ty(), variableMap[ast->value], ast->value);
    } else if (ast->type == "BinaryOp") {
        log::debug("Generating BinaryOp: " + ast->value);
        llvm::Value* lhs = generateExpression(ast->children[0], variableMap);
        llvm::Value* rhs = generateExpression(ast->children[1], variableMap);

        if (!lhs || !rhs) {
            handleError("Failed to generate operands for binary operation: " + ast->value);
        }

        if (ast->value == "+") {
            return builder.CreateAdd(lhs, rhs, "addtmp");
        } else if (ast->value == "-") {
            return builder.CreateSub(lhs, rhs, "subtmp");
        } else if (ast->value == "*") {
            return builder.CreateMul(lhs, rhs, "multmp");
        } else if (ast->value == "/") {
            return builder.CreateSDiv(lhs, rhs, "divtmp");
        } else {
            handleError("Unknown binary operator: " + ast->value);
        }
    }

    handleError("Unknown expression type: " + ast->type);
    return nullptr;
}


void printLLVMIR(const std::string& filename) {
    log::debug("Printing LLVM IR to file: " + filename);
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC);
    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message() << "\n";
        return;
    }
    module->print(dest, nullptr);
    dest.close();
}
