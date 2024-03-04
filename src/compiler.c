#include "compiler.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

CompileResult compile_var(Compiler* cs, Token* token) {
  Token* var_name = token->value.list.data[1];

  // Variable names must be identifiers
  if (var_name->type != TOKEN_IDENTIFIER) {
    array_append(cs->errs, &(Error){
                               .type = ERROR_EXPECTED_IDENTIFIER,
                           });
  }

  // Check if name is already taken
  if (is_defined(cs, &var_name->value.identifier)) {
    array_append(cs->errs, &(Error){
                               .type = ERROR_DUPLICATE_IDENT,
                           });
  }

  // Compile the right-hand side
  CompileResult cr = compile(cs, token->value.list.data[2]);

  // Store the identifer for future reference
  if (array_append(cs->idents, &(Identifier){
                                   .name = var_name->value.identifier,
                                   .type = cr.type,
                                   .reg = cr.reg,
                               }) != 0) {
    array_append(cs->errs, &(Error){
                               .type = ERROR_MALLOC,
                           });
  }

  return (CompileResult){
      .type = cr.type,
      .reg = move_to(cs, cr.reg, cs->stack++),
  };
}

CompileResult compile_do(Compiler* cs, Token* token) {
  enter_scope(cs);

  CompileResult cr;
  for (uint64_t i = 1; i < token->value.list.length; i++) {
    cr = compile(cs, token->value.list.data[i]);
  }

  leave_scope(cs);
  return cr;
}

CompileResult compile_binop(Compiler* cs, Token* token) {
  // Destruct
  Token* op = token->value.list.data[0];
  Token* lhs = token->value.list.data[1];
  Token* rhs = token->value.list.data[2];

  // Compile operands
  uint64_t save = cs->stack;
  CompileResult lhs_res = compile(cs, lhs);
  CompileResult rhs_res = compile(cs, rhs);
  cs->stack = save;  // Discard temp vars. They'll already be in cs->code

  // Check types
  if (lhs_res.type != rhs_res.type) {
    array_append(cs->errs, &(Error){
                               .type = ERROR_DIFF_TYPES,
                           });
  }

  // Code gen
  uint64_t dst = tmp(cs);  // Consume the next register
  array_append(cs->code, &(Instruction){
                             .type = INSTRUCTION_BINOP,
                             .value.binop =
                                 (InstructionBinOP){
                                     .op = op,
                                     .left = lhs_res,
                                     .right = rhs_res,
                                     .reg = dst,
                                 },
                         });

  return (CompileResult){
      .reg = dst,
      .type = lhs_res.type,
  };
}

CompileResult compile_list(Compiler* cs, Token* token) {
  uint64_t length = token->value.list.length;

  // Example: (+ 1 2)
  if (token_is_op(token->value.list.data[0]->type) && length == 3) {
    return compile_binop(cs, token);
  }

  // Example: (var a 1)
  if (token->value.list.data[0]->type == TOKEN_VAR && length == 3) {
    return compile_var(cs, token);
  }

  // Example: (do ...)
  if (token->value.list.data[0]->type == TOKEN_DO && length > 1) {
    return compile_do(cs, token);
  }

  return (CompileResult){
      .reg = -1,
      .type = TYPE_VOID,
  };
}

Compiler new_compiler(void) {
  Compiler compiler = (Compiler){
      .idents = array_new(10, sizeof(Identifier)),
      .scopes = array_new(10, sizeof(uint64_t)),
      .code = array_new(10, sizeof(Instruction)),
      .errs = array_new(10, sizeof(Error)),
      .stack = 0,
  };

  return compiler;
}

CompileResult compile(Compiler* cs, Token* token) {
  if (token == NULL) {
    array_append(cs->errs, &(Error){
                               .type = ERROR_EMPTY_PROGRAM,
                           });
  }

  switch (token->type) {
    case TOKEN_LIST: {
      return compile_list(cs, token);
      break;
    }
    case TOKEN_NUM: {
      uint64_t dst = tmp(cs);
      array_append(cs->code, &(Instruction){
                                 .type = INSTRUCTION_CONST,
                                 .value.constant =
                                     (InstructionConst){
                                         .reg = dst,
                                         .value = token->value.num,
                                     },
                             });
      return (CompileResult){
          .reg = dst,
          .type = TYPE_INT,
      };
      break;
    }

    default: {
      return (CompileResult){
          .reg = -1,
          .type = TYPE_VOID,
      };
      break;
    }
  }
}

void enter_scope(Compiler* cs) {
  size_t length = array_length(cs->idents);
  if (array_append(cs->scopes, &length) != 0) {
    array_append(cs->errs, &(Error){
                               .type = ERROR_MALLOC,
                           });
  }
}

void leave_scope(Compiler* cs) {
  size_t* current_scope = (size_t*)array_pop(cs->scopes);
  array_truncate(cs->idents, *current_scope);
}

bool is_defined(Compiler* cs, FatStr* str) {
  for (size_t i = 0; i < array_length(cs->idents); i++) {
    Identifier* cur = array_get(cs->idents, i);
    if (fatstr_cmp(&cur->name, str)) {
      return true;
    }
  }

  return false;
}

uint64_t tmp(Compiler* cs) { return cs->stack++; }

void instruction_to_string(Instruction* inst, char* buffer) {
  assert(inst != NULL);
  assert(buffer != NULL);

  switch (inst->type) {
    case INSTRUCTION_CONST: {
      char num[100];
      sprintf(num, "const %lld %lld\n", inst->value.constant.value,
              inst->value.constant.reg);
      strcat(buffer, num);
      break;
    }
    case INSTRUCTION_BINOP: {
      char tkn_buffer[100] = {0};
      token_to_string(inst->value.binop.op, tkn_buffer);
      char op[100] = {0};
      sprintf(op, "binop %s %lld %lld %lld\n", tkn_buffer,
              inst->value.binop.left.reg, inst->value.binop.right.reg,
              inst->value.binop.reg);
      strcat(buffer, op);
      break;
    }
    case INSTRUCTION_MOV: {
      sprintf(buffer, "mov %lld %lld\n", inst->value.mov.dst,
              inst->value.mov.reg);
      break;
    }
    default: {
      break;
    }
  }
}

uint64_t move_to(Compiler* cs, uint64_t reg, uint64_t dst) {
  if (reg != dst) {
    array_append(cs->code, &(Instruction){
                               .type = INSTRUCTION_MOV,
                               .value.mov =
                                   (InstructionMov){
                                       .reg = reg,
                                       .dst = dst,
                                   },
                           });
  }

  return dst;
}

bool ident_cmp(Identifier* i1, Identifier* i2) {
  return fatstr_cmp(&i1->name, &i2->name) && i1->reg == i2->reg &&
         i1->type == i2->type;
}

bool instruction_cmp(Instruction* i1, Instruction* i2) {
  if (i1->type != i2->type) {
    return false;
  }

  switch (i1->type) {
    case INSTRUCTION_CONST: {
      return i1->value.constant.reg == i2->value.constant.reg &&
             i1->value.constant.value == i2->value.constant.value;
      break;
    }
    case INSTRUCTION_BINOP: {
      return compile_result_cmp(&i1->value.binop.left, &i2->value.binop.left) &&
             tkncmp(i1->value.binop.op, i2->value.binop.op) &&
             i1->value.binop.reg == i2->value.binop.reg &&
             compile_result_cmp(&i1->value.binop.right, &i2->value.binop.right);
      break;
    }
    case INSTRUCTION_MOV: {
      return i1->value.mov.dst == i2->value.mov.dst &&
             i1->value.mov.reg == i2->value.mov.reg;
      break;
    }
    default: {
      return false;
      break;
    }
  }
}

bool compile_result_cmp(CompileResult* c1, CompileResult* c2) {
  return c1->reg == c2->reg && c1->type == c2->type;
}
