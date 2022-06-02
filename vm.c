#include "vm.h"
#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "memory.h"
#include "object.h"
#include "parser.h"
#include "table.h"
#include "value.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.ch->constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())

struct VM vm;

static void reset_stack() { vm.stack_top = vm.stack; }

void free_vm() {
  free_table(&vm.strings);
  free_table(&vm.globals);
  free_chunk(vm.ch);
  init_vm();
}

void init_vm() {
  vm.objects = NULL;
  init_table(&vm.strings);
  init_table(&vm.globals);
  reset_stack();
}

static void push(struct value val) {
  *vm.stack_top = val;
  vm.stack_top++;
}

static struct value pop() {
  vm.stack_top--;
  return *vm.stack_top;
}

bool static is_falsly(struct value val) {
  return (IS_BOOL(val) && AS_BOOL(val) == false) || IS_NIL(val) ||
         (IS_NUMBER(val) && AS_NUMBER(val) == 0);
}

struct value peek(int dist) {
  return vm.stack_top[-1 - dist];
}

static void runtime_error(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  size_t instruction = vm.ip - vm.ch->code - 1;
  int line = vm.ch->lines[instruction];
  fprintf(stderr, "[line %d] in script\n", line);
  reset_stack();
}

static void concat() {
  struct obj_string *str2 = AS_STRING(pop());
  struct obj_string *str1 = AS_STRING(pop());
  int length = str1->length + str2->length;
  char *chars = ALLOC(char, length + 1);
  memcpy(chars, str1->chars, str1->length);
  memcpy(chars + str1->length, str2->chars, str2->length);
  chars[length] = '\0';
  struct obj_string *concat_res = take_str(chars, length);
  push(OBJ_VAL(concat_res));
}

#define BINARY_OP(op, WHAT_VAL)                                                \
  do {                                                                         \
    if (!IS_NUMBER(peek(0)) && !IS_NUMBER(peek(1))) {                          \
      runtime_error("Operand must be a number");                               \
      return INTERPRET_RUNTIME_ERROR;                                          \
    }                                                                          \
    double b = AS_NUMBER(pop());                                               \
    double a = AS_NUMBER(pop());                                               \
    push(WHAT_VAL(a op b));                                                    \
  } while (false);

static enum interpret_result run() {
  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("      ");
    for (struct value *slot = vm.stack; slot < vm.stack_top; slot++) {
      printf("[ ");
      print_value(*slot);
      printf(" ]");
    }
    printf("\n");
    disassemble_instruction(vm.ch, vm.ip - vm.ch->code);
#endif
    uint8_t instruction = READ_BYTE();
    switch (instruction) {
    case OP_CONSTANT: {
      struct value val = vm.ch->constants.values[READ_BYTE()];
      push(val);
      break;
    }
    case OP_NEGATE: {
      struct value a = pop();
      push(NUMBER_VAL(-AS_NUMBER(a)));
      break;
    }
    case OP_ADD: {
      if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
        concat();
      } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
        BINARY_OP(+, NUMBER_VAL);
      } else {
        runtime_error("operands dont match!");
        return INTERPRET_RUNTIME_ERROR;
      }
      break;
    }
    case OP_SUBTRACT:
      BINARY_OP(-, NUMBER_VAL);
      break;
    case OP_MULTIPLY:
      BINARY_OP(*, NUMBER_VAL);
      break;
    case OP_DIVIDE:
      BINARY_OP(/, NUMBER_VAL);
      break;
    case OP_EQUAL: {
      struct value a = pop();
      struct value b = pop();
      push(BOOL_VAL(AS_NUMBER(a) == AS_NUMBER(b)));
      break;
    }
    case OP_GREATER:
      BINARY_OP(>, BOOL_VAL);
      break;
    case OP_LESS:
      BINARY_OP(<, BOOL_VAL);
      break;
    case OP_NOT:
      push(BOOL_VAL(is_falsly(pop())));
      break;
    case OP_POP:
      pop();
      break;
    case OP_WRITE:
      print_value(pop());
      break;
    case OP_DEFINE_GLOBAL: {
      struct obj_string *name = READ_STRING();
      table_set(&vm.globals, name, peek(0));
      pop();
      break;
    }

    case OP_GET_GLOBAL: {
      struct obj_string *name = READ_STRING();
      struct value v;
      if (!table_get(&vm.globals, name, &v)) {
        runtime_error("Undefined variable '%s'.", name->chars);
        return INTERPRET_RUNTIME_ERROR;
      }
      push(v);
      break;
    }
    case OP_SET_GLOBAL: {
		struct obj_string *name = READ_STRING();
		if (table_set(&vm.globals,name,peek(0))){
			table_del(&vm.globals,name);
			runtime_error("Undefined variable '%s'",name->chars);
			return INTERPRET_RUNTIME_ERROR;
		}
		break;
    }
	case OP_SET_LOCAL: {
		uint8_t index = READ_BYTE();
		vm.stack[index] = peek(0);
		break;
	}
	case OP_GET_LOCAL: {
		uint8_t index = READ_BYTE();
		push(vm.stack[index]);
		break;
	}
    case OP_RETURN:
      return INTERPRET_OK;
    }
  }
}

enum interpret_result interpret(const char *source) {
  struct chunk ch;
  init_chunk(&ch);
  if (!compile(&ch, source)) {
    free_chunk(&ch);
    return INTERPRET_COMPILE_ERROR;
  }
  vm.ch = &ch;
  vm.ip = ch.code;
  enum interpret_result result = run();
  return result;
}
