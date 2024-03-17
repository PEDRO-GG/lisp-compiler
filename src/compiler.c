#include "compiler.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

void enter_scope(Compiler* cs) {
  cs->scope = new_scope(cs->scope);
  cs->scope->save = cs->stack;
}

void leave_scope(Compiler* cs) {
  cs->stack = cs->scope->save;
  cs->nvar -= cs->scope->nlocal;
  cs->scope = cs->scope->prev;
}

uint64_t add_var(Compiler* cs, FatStr name, ValueType type) {
  if (is_defined(cs, &name)) {
    perror("duplicated name");
    exit(EXIT_FAILURE);
  }

  Identifier* to_add = &(Identifier){
      .name = name,
      .cr =
          (CompileResult){
              .type = type,
              .dst = cs->nvar,
          },
  };

  if (array_append(cs->scope->names, to_add) != 0) {
    perror("failed to append to array");
    exit(EXIT_FAILURE);
  }

  cs->scope->nlocal++;

  assert(cs->stack == cs->nvar);

  uint64_t dst = cs->stack;
  cs->stack++;
  cs->nvar++;
  return dst;
}

CompileResult get_var(Compiler* cs, FatStr name) {
  CompileResult cr = scope_get_var(cs->scope, name);
  if (cr.dst >= 0) return cr;
  perror("undefined name");
  exit(EXIT_FAILURE);
}

uint64_t tmp(Compiler* cs) {
  uint64_t dst = cs->stack;
  cs->stack++;
  return dst;
}

size_t new_label(Compiler* cs) {
  size_t length = array_length(cs->labels);
  array_append(cs->labels, &length);
  return length;
}

void set_label(Compiler* cs, Label label) {
  size_t labels_length = array_length(cs->labels);
  assert(label < labels_length);

  size_t* to_change = array_get(cs->labels, label);
  size_t code_length = array_length(cs->code);
  (*to_change) = code_length;
}

Scope* new_scope(Scope* prev) {
  Scope* scope = malloc(sizeof(Scope));
  if (scope == NULL) {
    perror("memory allocation failed");
    exit(EXIT_FAILURE);
  }

  scope->prev = prev;
  scope->nlocal = 0;
  scope->save = 0;
  scope->loop_start = prev ? prev->loop_start : -1;
  scope->loop_end = prev ? prev->loop_end : -1;

  scope->names = array_new(20, sizeof(Identifier));
  if (scope->names == NULL) {
    perror("memory allocation failed");
    exit(EXIT_FAILURE);
  }

  return scope;
}

CompileResult scope_get_var(Scope* scope, FatStr name) {
  while (scope) {
    Array* names = scope->names;
    size_t names_length = array_length(scope->names);

    for (size_t i = 0; i < names_length; i++) {
      Identifier* cur = array_get(names, i);
      if (fatstr_cmp(&cur->name, &name)) {
        return cur->cr;
      }
    }

    scope = scope->prev;
  }

  return (CompileResult){
      .dst = -1,
      .type = TYPE_VOID,
  };
}

CompileResult compile_expr(Compiler* cs, Token* token, bool allow_vars) {
  if (allow_vars) {
    assert(cs->stack == cs->nvar);
  }
  uint64_t save = cs->stack;

  CompileResult cr = compile_expr_tmp(cs, token, allow_vars);
  assert(cr.dst < cs->stack);

  if (allow_vars) {
    cs->stack = cs->nvar;
  } else {
    cs->stack = save;
  }

  assert(cr.dst <= cs->stack);
  return cr;
}

CompileResult compile_const(Compiler* cs, Token* token) {
  assert(token->type == TOKEN_NUM);
  uint64_t dst = tmp(cs);
  array_append(cs->code, &(Instruction){
                             .type = INSTRUCTION_CONST,
                             .value.constant =
                                 (InstructionConst){
                                     .dst = dst,
                                     .value = token->value.num,
                                 },
                         });
  return (CompileResult){
      .dst = dst,
      .type = TYPE_INT,
  };
}

CompileResult compile_binop(Compiler* cs, Token* token) {
  // Destruct
  Token* op = token->value.list.data[0];
  Token* lhs = token->value.list.data[1];
  Token* rhs = token->value.list.data[2];

  // Compile operands
  uint64_t save = cs->stack;
  CompileResult lhs_res = compile_expr_tmp(cs, lhs, false);
  CompileResult rhs_res = compile_expr_tmp(cs, rhs, false);
  cs->stack = save;  // Discard temp vars. They'll already be in cs->code

  // Check types
  if (lhs_res.type != rhs_res.type) {
    perror("incompatiable types");
    exit(EXIT_FAILURE);
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
                                     .dst = dst,
                                 },
                         });

  return (CompileResult){
      .dst = dst,
      .type = lhs_res.type,
  };
}

CompileResult compile_expr_tmp(Compiler* cs, Token* token, bool allow_vars) {
  // Example: a
  if (token->type == TOKEN_IDENTIFIER) {
    return get_var(cs, token->value.identifier);
  }

  // Example: 1
  if (token->type == TOKEN_NUM) {
    return compile_const(cs, token);
  }

  assert(token->type == TOKEN_LIST);
  uint64_t length = token->value.list.length;
  if (length == 0) {
    perror("empty list");
    exit(EXIT_FAILURE);
  }

  // Example: (+ 1 2)
  if (length == 3 && token_is_op(token->value.list.data[0]->type)) {
    return compile_binop(cs, token);
  }

  // Example: (do ...)
  if (token->value.list.data[0]->type == TOKEN_DO) {
    return compile_scope(cs, token);
  }

  // Example: (var a 1)
  if (length == 3 && token->value.list.data[0]->type == TOKEN_VAR) {
    if (!allow_vars) {
      perror("variable declaration not allowed here");
      exit(EXIT_FAILURE);
    }
    return compile_var(cs, token);
  }

  // Example: (set a 1)
  if (length == 3 && token->value.list.data[0]->type == TOKEN_SET) {
    return compile_set(cs, token);
  }

  // Example: (if condition yes)
  // Example: (if condition yes no)
  if ((length == 3 || length == 4) &&
      token->value.list.data[0]->type == TOKEN_IF) {
    return compile_if(cs, token);
  }

  // Example: (loop condition body)
  if (length == 3 && token->value.list.data[0]->type == TOKEN_LOOP) {
    return compile_loop(cs, token);
  }

  // Example: break
  if (token->value.list.data[0]->type == TOKEN_BREAK) {
    if (cs->scope->loop_end < 0) {
      perror("`break` outside a loop");
      exit(EXIT_FAILURE);
    }
    array_append(cs->code, &(Instruction){
                               .type = INSTRUCTION_JMP,
                               .value.jmp =
                                   (InstructionJmp){
                                       .label = cs->scope->loop_end,
                                   },
                           });
    return (CompileResult){
        .dst = -1,
        .type = TYPE_VOID,
    };
  }

  // Example: continue
  if (token->value.list.data[0]->type == TOKEN_CONTINUE) {
    if (cs->scope->loop_start < 0) {
      perror("`continue` outside a loop");
      exit(EXIT_FAILURE);
    }
    array_append(cs->code, &(Instruction){
                               .type = INSTRUCTION_JMP,
                               .value.jmp =
                                   (InstructionJmp){
                                       .label = cs->scope->loop_start,
                                   },
                           });
    return (CompileResult){
        .dst = -1,
        .type = TYPE_VOID,
    };
  }

  perror("unknown expression");
  exit(EXIT_FAILURE);
}

CompileResult compile_scope(Compiler* cs, Token* token) {
  enter_scope(cs);

  CompileResult cr = (CompileResult){
      .dst = -1,
      .type = TYPE_VOID,
  };

  for (uint64_t i = 1; i < token->value.list.length; i++) {
    cr = compile_expr(cs, token->value.list.data[i], true);
  }

  leave_scope(cs);

  if (cr.dst >= cs->stack) {
    cr.dst = move_to(cs, cr.dst, tmp(cs));
  }

  return cr;
}

uint64_t move_to(Compiler* cs, uint64_t src, uint64_t dst) {
  if (src != dst) {
    array_append(cs->code, &(Instruction){
                               .type = INSTRUCTION_MOV,
                               .value.mov =
                                   (InstructionMov){
                                       .src = src,
                                       .dst = dst,
                                   },
                           });
  }

  return dst;
}

CompileResult compile_var(Compiler* cs, Token* token) {
  Token* name = token->value.list.data[1];
  Token* value = token->value.list.data[2];

  // Compile the right-hand side
  CompileResult cr = compile_expr(cs, value, false);
  if (cr.dst < 0) {
    perror("unable to parse expression");
    exit(EXIT_FAILURE);
  }

  // Variable names must be identifiers
  if (name->type != TOKEN_IDENTIFIER) {
    perror("name must be an identifier");
    exit(EXIT_FAILURE);
  }

  uint64_t dst = add_var(cs, name->value.identifier, cr.type);
  return (CompileResult){
      .type = cr.type,
      .dst = move_to(cs, cr.dst, dst),
  };
}

CompileResult compile_set(Compiler* cs, Token* token) {
  Token* name = token->value.list.data[1];
  Token* value = token->value.list.data[2];

  CompileResult dst_cr = get_var(cs, name->value.identifier);
  CompileResult cr = compile_expr(cs, value, false);
  if (dst_cr.type != cr.type) {
    perror("right-hand side type does not match expected type");
    exit(EXIT_FAILURE);
  }
  return (CompileResult){
      .type = dst_cr.type,
      .dst = move_to(cs, cr.dst, dst_cr.dst),
  };
}

CompileResult compile_if(Compiler* cs, Token* token) {
  uint64_t length = token->value.list.length;
  Token* cond = token->value.list.data[1];
  Token* yes = token->value.list.data[2];
  Token* no = NULL;
  if (length == 4) {
    no = token->value.list.data[3];
  }

  Label l_true = new_label(cs);
  Label l_false = new_label(cs);

  enter_scope(cs);

  // Compile the condition
  CompileResult cond_cr = compile_expr(cs, cond, true);
  if (cond_cr.type == TYPE_VOID) {
    perror("expect boolean condition");
    exit(EXIT_FAILURE);
  }

  array_append(cs->code, &(Instruction){
                             .type = INSTRUCTION_JMPF,
                             .value.jmpf =
                                 (InstructionJmpf){
                                     // go to `else` if false
                                     .dst = cond_cr.dst,
                                     .label = l_false,
                                 },
                         });

  // Compile the if body
  CompileResult body_cr = compile_expr(cs, yes, false);
  if (body_cr.dst >= 0) {
    move_to(cs, body_cr.dst, cs->stack);
  }

  CompileResult else_cr = (CompileResult){
      .dst = -1,
      .type = TYPE_VOID,
  };

  // Add JMP
  if (no) {
    array_append(cs->code, &(Instruction){
                               .type = INSTRUCTION_JMP,
                               .value.jmp =
                                   (InstructionJmp){
                                       // skip `else` if true
                                       .label = l_true,
                                   },
                           });
  }

  set_label(cs, l_false);

  // Compile else
  if (no) {
    else_cr = compile_expr(cs, no, false);
    if (else_cr.dst >= 0) {
      move_to(cs, else_cr.dst, cs->stack);
    }
  }

  set_label(cs, l_true);

  leave_scope(cs);

  if (body_cr.dst < 0 || else_cr.dst < 0 || body_cr.type != else_cr.type) {
    return (CompileResult){
        .dst = -1,
        .type = TYPE_VOID,
    };
  }

  return (CompileResult){
      .dst = tmp(cs),
      .type = body_cr.type,
  };
}

CompileResult compile_loop(Compiler* cs, Token* token) {
  Token* cond = token->value.list.data[1];
  Token* body = token->value.list.data[2];

  cs->scope->loop_start = new_label(cs);
  cs->scope->loop_end = new_label(cs);

  enter_scope(cs);

  set_label(cs, cs->scope->loop_start);

  // Compile the condition
  CompileResult cond_cr = compile_expr(cs, cond, true);
  if (cond_cr.dst < 0) {
    perror("expect boolean condition");
    exit(EXIT_FAILURE);
  }

  // Add the jmpf
  array_append(cs->code, &(Instruction){
                             .type = INSTRUCTION_JMPF,
                             .value.jmpf =
                                 (InstructionJmpf){
                                     .dst = cond_cr.dst,
                                     .label = cs->scope->loop_end,
                                 },
                         });

  // Compile the body
  compile_expr(cs, body, true);
  array_append(cs->code, &(Instruction){
                             .type = INSTRUCTION_JMP,
                             .value.jmp =
                                 (InstructionJmp){
                                     .label = cs->scope->loop_start,
                                 },
                         });

  set_label(cs, cs->scope->loop_end);
  leave_scope(cs);

  return (CompileResult){
      .dst = -1,
      .type = TYPE_VOID,
  };
}

Compiler new_compiler(void) {
  Compiler compiler = (Compiler){
      .scope = new_scope(NULL),
      .code = array_new(10, sizeof(Instruction)),
      .labels = array_new(10, sizeof(Label)),
      .nvar = 0,
      .stack = 0,
  };

  return compiler;
}

bool is_defined(Compiler* cs, FatStr* str) {
  Array* names = cs->scope->names;
  size_t names_length = array_length(names);

  for (size_t i = 0; i < names_length; i++) {
    Identifier* cur = array_get(names, i);
    if (fatstr_cmp(&cur->name, str)) {
      return true;
    }
  }

  return false;
}

void instruction_to_string(Instruction* inst, char* buffer) {
  assert(inst != NULL);
  assert(buffer != NULL);

  switch (inst->type) {
    case INSTRUCTION_CONST: {
      char num[100];
      sprintf(num, "const %lld %lld\n", inst->value.constant.value,
              inst->value.constant.dst);
      strcat(buffer, num);
      break;
    }
    case INSTRUCTION_BINOP: {
      char tkn_buffer[100] = {0};
      token_to_string(inst->value.binop.op, tkn_buffer);
      char op[100] = {0};
      sprintf(op, "binop %s %lld %lld %lld\n", tkn_buffer,
              inst->value.binop.left.dst, inst->value.binop.right.dst,
              inst->value.binop.dst);
      strcat(buffer, op);
      break;
    }
    case INSTRUCTION_MOV: {
      char mov[100] = {0};
      sprintf(mov, "mov %lld %lld\n", inst->value.mov.src, inst->value.mov.dst);
      strcat(buffer, mov);
      break;
    }
    case INSTRUCTION_JMP: {
      char jmp[100] = {0};
      sprintf(jmp, "jmp L%zu\n", inst->value.jmp.label);
      strcat(buffer, jmp);
      break;
    }
    case INSTRUCTION_JMPF: {
      char jmpf[100] = {0};
      sprintf(jmpf, "jmpf %llu L%zu\n", inst->value.jmpf.dst,
              inst->value.jmpf.label);
      strcat(buffer, jmpf);
      break;
    }
    default: {
      break;
    }
  }
}

Array* dump_ir(Compiler* cs) {
  Array* buffer = array_new(1024, sizeof(char));  // Holds our string

  for (size_t i = 0; i < array_length(cs->code); i++) {
    // Check if we should place a label
    for (size_t j = 0; j < array_length(cs->labels); j++) {
      Label* l = array_get(cs->labels, j);
      if (*l == i) {
        array_append_fmt(buffer, "L%zu:\n", j);
      }
    }

    // Stringify the instruction
    Instruction* inst = array_get(cs->code, i);
    char inst_buffer[100] = {0};
    instruction_to_string(inst, inst_buffer);
    array_append_fmt(buffer, "%s", inst_buffer);
  }

  // Place the last label
  for (size_t i = 0; i < array_length(cs->labels); i++) {
    Label* l = array_get(cs->labels, i);
    if (*l == array_length(cs->code)) {
      array_append_fmt(buffer, "L%zu:\n", i);
      break;
    }
  }

  return buffer;
}

bool ident_cmp(Identifier* i1, Identifier* i2) {
  return fatstr_cmp(&i1->name, &i2->name) &&
         compile_result_cmp(&i1->cr, &i2->cr);
}

bool instruction_cmp(Instruction* i1, Instruction* i2) {
  if (i1->type != i2->type) {
    return false;
  }

  switch (i1->type) {
    case INSTRUCTION_CONST: {
      return i1->value.constant.dst == i2->value.constant.dst &&
             i1->value.constant.value == i2->value.constant.value;
      break;
    }
    case INSTRUCTION_BINOP: {
      return compile_result_cmp(&i1->value.binop.left, &i2->value.binop.left) &&
             tkncmp(i1->value.binop.op, i2->value.binop.op) &&
             i1->value.binop.dst == i2->value.binop.dst &&
             compile_result_cmp(&i1->value.binop.right, &i2->value.binop.right);
      break;
    }
    case INSTRUCTION_MOV: {
      return i1->value.mov.dst == i2->value.mov.dst &&
             i1->value.mov.dst == i2->value.mov.dst;
      break;
    }
    default: {
      return false;
      break;
    }
  }
}

bool compile_result_cmp(CompileResult* c1, CompileResult* c2) {
  return c1->dst == c2->dst && c1->type == c2->type;
}

bool scope_cmp(Scope* s1, Scope* s2) {
  if (s1 == NULL && s2 == NULL) {
    return true;
  }

  if ((s1 == NULL && s2 != NULL) || (s1 != NULL && s2 == NULL)) {
    return false;
  }

  if (s1->nlocal != s2->nlocal || s1->save != s2->save) {
    return false;
  }

  size_t length1 = array_length(s1->names);
  size_t length2 = array_length(s2->names);

  if (length1 != length2) {
    return false;
  }

  for (size_t i = 0; i < length1; i++) {
    Identifier* ident1 = array_get(s1->names, i);
    Identifier* ident2 = array_get(s2->names, i);
    if (!ident_cmp(ident1, ident2)) {
      return false;
    }
  }

  return scope_cmp(s1->prev, s2->prev);
}

bool compiler_cmp(Compiler* c1, Compiler* c2) {
  if (!scope_cmp(c1->scope, c2->scope)) {
    return false;
  }

  uint64_t code_length1 = array_length(c1->code);
  uint64_t code_length2 = array_length(c2->code);

  if (code_length1 != code_length2 || c1->stack != c2->stack ||
      c1->nvar != c2->nvar) {
    return false;
  }

  // Compare each instruction
  for (size_t i = 0; i < code_length1; i++) {
    Instruction* inst1 = array_get(c1->code, i);
    Instruction* inst2 = array_get(c2->code, i);

    if (!instruction_cmp(inst1, inst2)) {
      return false;
    }
  }

  return true;
}
