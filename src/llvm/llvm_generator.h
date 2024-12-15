#ifndef LLVM_GENERATOR_H
#define LLVM_GENERATOR_H

#include "../parser/parser.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

void initializeLLVM();

void generateLLVMIR(ASTNode* ast);

void printLLVMIR(const std::string& filename);

#endif // LLVM_GENERATOR_H
