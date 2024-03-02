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
typedef struct InstructionMov InstructionMov;

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
  uint64_t reg;
} Identifier;

typedef struct {
  ValueType type;
  uint64_t reg;
} CompileResult;

typedef enum {
  INSTRUCTION_CONST,
  INSTRUCTION_BINOP,
  INSTRUCTION_MOV,
} InstructionType;

struct InstructionConst {
  int64_t value;
  uint64_t reg;
};

struct InstructionMov {
  uint64_t reg;
  uint64_t dst;
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
    struct InstructionMov mov;
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
uint64_t move_to(Compiler* cs, uint64_t reg, uint64_t dst);

#endif  // COMPILER_H
