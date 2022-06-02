#include "chunk.h"
#include "value.h"
#include "common.h"
#include "memory.h"

void 
init_chunk(struct chunk* c){
	c->size = 0;
	c->capacity = 0;
	c->code = NULL;
	c->lines = NULL;
	init_valuearray(&c->constants);
}

void 
free_chunk(struct chunk* c){
	FREE_ARRAY(c->code,uint8_t,c->capacity);
	FREE_ARRAY(c->lines,int,c->capacity);
	free_valuearray(&c->constants);
	init_chunk(c);
}

void 
write_chunk(struct chunk* c,uint8_t op_code,int line){
	if (c->size + 1 > c->capacity){
		int old_cap = c->capacity;	
		c->capacity = GROW_CAP(old_cap);
		c->code = GROW_ARRAY(c->code,uint8_t,old_cap,c->capacity);
		c->lines = GROW_ARRAY(c->lines,int,old_cap,c->capacity);
	}
	c->code[c->size] = op_code;
	c->lines[c->size] = line;
	c->size++;
}

int 
add_constant(struct chunk* c ,struct value v){
	write_valuearray(&c->constants,v);
	return c->constants.count - 1;
}
