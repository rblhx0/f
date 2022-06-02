#ifndef debug_h
#define debug_h
	
#include "chunk.h"
#include "common.h"

#define DEBUG_TRACE_EXECUTION

void disassemble_chunk(struct chunk*,const char*);
int disassemble_instruction(struct chunk*,int);

#endif
