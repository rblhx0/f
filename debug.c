#include "debug.h"
#include "chunk.h"
#include "common.h"
#include "value.h"
#include <stdio.h>

static int constant_instruction(const char *msg, struct chunk *ch, int offset) {
	uint8_t constant = ch->code[offset + 1];
	printf("%s  %04d   ", msg, constant);
	print_value(ch->constants.values[constant]);
	printf("\n");
	return offset + 2;
}

static int simple_instruction(const char *msg, int offset) {
	printf("%s\n", msg);
	return offset + 1;
}

void disassemble_chunk(struct chunk *ch, const char *name) {
	printf("== %s ==\n", name);
	for (int offset = 0; offset < ch->size;) {
		offset = disassemble_instruction(ch, offset);
	}
}

int disassemble_instruction(struct chunk *ch, int offset) {
	printf("%04d ", offset);
	if (offset > 0 && ch->lines[offset] == ch->lines[offset - 1]) {
		printf("   | ");
	} else {
		printf("%04d ", ch->lines[offset]);
	}

	uint8_t instruction = ch->code[offset];
	switch (instruction) {
		case OP_ADD:
			return simple_instruction("OP_ADD", offset);
		case OP_DIVIDE:
			return simple_instruction("OP_DIVIDE", offset);
		case OP_SUBTRACT:
			return simple_instruction("OP_SUBTRACT", offset);
		case OP_MULTIPLY:
			return simple_instruction("OP_MULTIPLY", offset);
		case OP_GREATER:
			return simple_instruction("OP_GREATER", offset);
		case OP_LESS:
			return simple_instruction("OP_LESS", offset);
		case OP_FALSE:
			return simple_instruction("OP_FALSE", offset);
		case OP_EQUAL:
			return simple_instruction("OP_EQUAL", offset);
		case OP_NOT:
			return simple_instruction("OP_NOT", offset);
		case OP_NEGATE:
			return simple_instruction("OP_NEGATE", offset);
		case OP_CONSTANT:
			return constant_instruction("OP_CONSTANT", ch, offset);
		case OP_RETURN:
			return simple_instruction("OP_RETURN", offset);
		case OP_POP:
			return simple_instruction("OP_POP", offset);
		case OP_WRITE:
			return simple_instruction("OP_WRITE", offset);
		case OP_DEFINE_GLOBAL:
			return constant_instruction("OP_DEFINE_GLOBAL", ch, offset);
		case OP_GET_GLOBAL:
			return constant_instruction("OP_GET_GLOBAL", ch, offset);
		case OP_SET_GLOBAL:
			return constant_instruction("OP_SET_GLOBAL", ch, offset);
		case OP_SET_LOCAL:
			return constant_instruction("OP_SET_LOCAL", ch, offset);
		case OP_GET_LOCAL:
			return constant_instruction("OP_GET_LOCAL", ch, offset);
		default:
			printf("unknown opcode\n");
			return offset + 1;
	}
}
