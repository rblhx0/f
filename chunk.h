#ifndef chunk_h
#define chunk_h

#include "value.h"
#include "common.h"

enum op_code {
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_TRUE,
	OP_FALSE,
	OP_NEGATE,
	OP_NOT,
	OP_GREATER,
	OP_LESS,
	OP_EQUAL,
	OP_RETURN,
	OP_CONSTANT,
	OP_POP,
	OP_WRITE,
	OP_DEFINE_GLOBAL,
	OP_GET_GLOBAL,
	OP_SET_GLOBAL,
	OP_SET_LOCAL,
	OP_GET_LOCAL,
};

struct chunk {
	uint8_t* code;	
	int size;
	int capacity;
	int* lines;
	struct value_array constants;
};


void init_chunk(struct chunk*);
void free_chunk(struct chunk*);
void write_chunk(struct chunk*,uint8_t,int);
int add_constant(struct chunk*,struct value);

#endif
