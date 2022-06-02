#ifndef vm_h
#define vm_h


#include "common.h"
#include "chunk.h"
#include "value.h"
#include "object.h"
#include "table.h"

#define STACK_MAX 256

struct VM {
	struct chunk* ch;
	uint8_t* ip;
	struct value stack[STACK_MAX];
	struct value* stack_top;
	struct object* objects;
	struct table strings;
	struct table globals;
};

enum interpret_result {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
};

enum interpret_result interpret(const char*);
void init_vm();
void free_vm();


#endif
