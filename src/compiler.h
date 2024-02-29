#ifndef COMPILER_H
#define COMPILER_H
#include <stdint.h>

#include "array.h"
#include "errors.h"
#include "fatstr.h"
#include "token.h"

typedef struct Instruction Instruction;
typedef struct InstructionConst InstructionConst;
typedef struct InstructionBinOP InstructionBinOP;

typedef enum {
  TYPE_INT,
  TYPE_FUNC,
  TYPE_BYTE,
  TYPE_BYTE_PTR,
  TYPE_VOID,
} ValueType;

typedef struct {
  FatStr name;
  ValueType type;
} Identifier;

typedef struct {
  ValueType type;
  uint64_t reg;
} CompileResult;

typedef enum {
  INSTRUCTION_CONST,
  INSTRUCTION_BINOP,
} InstructionType;

struct InstructionConst {
  int64_t value;
  uint64_t reg;
};

struct InstructionBinOP {
  CompileResult left;
  CompileResult right;
  Token* op;
  uint64_t reg;
};

struct Instruction {
  InstructionType type;
  union {
    struct InstructionConst constant;
    struct InstructionBinOP binop;
  } value;
};

typedef struct {
  Array* idents;   // array of identifiers
  Array* scopes;   // array of indices from idents
  Array* code;     // array of instructions
  Array* errs;     // array of errors
  uint64_t stack;  // next available register
} Compiler;

Compiler new_compiler(void);
CompileResult compile(Compiler* cs, Token* token);
void enter_scope(Compiler* cs);
void leave_scope(Compiler* cs);
bool is_defined(Compiler* cs, FatStr* str);
uint64_t tmp(Compiler* cs);
void instruction_to_string(Instruction* inst, char* buffer);

#endif  // COMPILER_H
