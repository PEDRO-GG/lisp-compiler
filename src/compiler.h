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
typedef struct InstructionJmp InstructionJmp;
typedef struct InstructionJmpf InstructionJmpf;
typedef size_t Label;

typedef enum {
  TYPE_INT,
  TYPE_FUNC,
  TYPE_BYTE,
  TYPE_BYTE_PTR,
  TYPE_VOID,
} ValueType;

typedef struct {
  ValueType type;
  int64_t dst;
} CompileResult;

typedef struct {
  FatStr name;
  CompileResult cr;
} Identifier;

typedef enum {
  INSTRUCTION_CONST,
  INSTRUCTION_BINOP,
  INSTRUCTION_MOV,
  INSTRUCTION_JMP,
  INSTRUCTION_JMPF,
} InstructionType;

struct InstructionConst {
  int64_t value;
  uint64_t dst;
};

struct InstructionMov {
  uint64_t src;
  uint64_t dst;
};

struct InstructionJmp {
  Label label;
};

struct InstructionJmpf {
  uint64_t dst;
  Label label;
};

struct InstructionBinOP {
  CompileResult left;
  CompileResult right;
  Token* op;
  uint64_t dst;
};

struct Instruction {
  InstructionType type;
  union {
    struct InstructionConst constant;
    struct InstructionBinOP binop;
    struct InstructionMov mov;
    struct InstructionJmp jmp;
    struct InstructionJmpf jmpf;
  } value;
};

typedef struct Scope {
  struct Scope* prev;
  uint64_t nlocal;
  uint64_t save;
  Array* names;  // []Identifier
  uint64_t loop_start;
  uint64_t loop_end;
} Scope;

typedef struct {
  Scope* scope;   // Current scope
  Array* code;    // []Instruction
  Array* labels;  // []Label
  int64_t nvar;
  int64_t stack;
} Compiler;

void enter_scope(Compiler* cs);
void leave_scope(Compiler* cs);
uint64_t add_var(Compiler* cs, FatStr name, ValueType type);
CompileResult get_var(Compiler* cs, FatStr name);
uint64_t tmp(Compiler* cs);
size_t new_label(Compiler* cs);
void set_label(Compiler* cs, Label label);
Scope* new_scope(Scope* prev);
CompileResult scope_get_var(Scope* scope, FatStr name);
CompileResult compile_expr(Compiler* cs, Token* token, bool allow_vars);
CompileResult compile_const(Compiler* cs, Token* token);
CompileResult compile_binop(Compiler* cs, Token* token);
CompileResult compile_expr_tmp(Compiler* cs, Token* token, bool allow_vars);
CompileResult compile_scope(Compiler* cs, Token* token);
uint64_t move_to(Compiler* cs, uint64_t src, uint64_t dst);
CompileResult compile_var(Compiler* cs, Token* token);
CompileResult compile_set(Compiler* cs, Token* token);
CompileResult compile_if(Compiler* cs, Token* token);
CompileResult compile_loop(Compiler* cs, Token* token);
Compiler new_compiler(void);
bool is_defined(Compiler* cs, FatStr* str);
void instruction_to_string(Instruction* inst, char* buffer);
Array* dump_ir(Compiler* cs);
bool ident_cmp(Identifier* i1, Identifier* i2);
bool instruction_cmp(Instruction* i1, Instruction* i2);
bool compile_result_cmp(CompileResult* c1, CompileResult* c2);
bool scope_cmp(Scope* s1, Scope* s2);
bool compiler_cmp(Compiler* c1, Compiler* c2);

#endif  // COMPILER_H
