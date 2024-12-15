#include "llvm_generator.h"
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Verifier.h>
#include <fstream>
#include "llvm_generator.h"
#include "../logger/logger.h"

llvm::LLVMContext context;
llvm::IRBuilder<> builder(context);
llvm::Module* module = nullptr;

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
        llvm::Type* returnType = builder.getInt32Ty();
        llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, false);
        llvm::Function* function = llvm::Function::Create(
            funcType, llvm::Function::ExternalLinkage, ast->value, module);

        if (!function) {
            log::error("Failed to create LLVM function: " + ast->value);
            return;
        }

        llvm::BasicBlock* block = llvm::BasicBlock::Create(context, "entry", function);
        builder.SetInsertPoint(block);

        for (size_t i = 0; i < ast->children.size(); ++i) {
            auto* child = ast->children[i];
            if (!child) {
                log::error("Child ASTNode is null at index: " + std::to_string(i));
                continue;
            }

            log::debug("Processing child node type: " + child->type);
            if (child->type == "ReturnStatement") {
                if (child->children.empty()) {
                    log::error("ReturnStatement node has no children");
                    continue;
                }

                if (child->children[0]->type == "Literal") {
                    const std::string& valueStr = child->children[0]->value;
                    bool isNumeric = !valueStr.empty() &&
                                     std::all_of(valueStr.begin(), valueStr.end(), ::isdigit);

                    if (!isNumeric) {
                        log::error("Invalid return value: " + valueStr);
                        continue;
                    }

                    int value = std::stoi(valueStr);
                    builder.CreateRet(builder.getInt32(value));
                    log::debug("Return value added to function: " + std::to_string(value));
                } else {
                    log::error("Unexpected child type in ReturnStatement: " + child->children[0]->type);
                }
            } else {
                log::debug("Unhandled child node type: " + child->type);
            }
        }

        if (!function->back().getTerminator()) {
            builder.CreateRetVoid();
            log::debug("Added fallback return void terminator for function: " + ast->value);
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
