#include "object.h"
#include "common.h"
#include "memory.h"
#include "string.h"
#include "table.h"
#include "value.h"
#include "vm.h"
#include <stdint.h>
#include <stdio.h>

#define OBJ(type,f) \
	(type*)alloc_object(sizeof(type),f);

extern struct VM vm;

static uint32_t hash(const char* str, int length){
	uint32_t hash = 2166136261u;
	for (int i = 0 ; i <length;i++) {
		hash ^= str[i];
		hash *= 16777619;
	}
	return hash;
}

static struct object* alloc_object(size_t size,enum obj_type type) {
	struct object* new_obj = (struct object*)f_realloc(NULL,0,size);
	new_obj->type = type;
	new_obj->next = vm.objects;
	vm.objects = new_obj;
	return new_obj;
}


static struct obj_string* alloc_string(char* chars, size_t length){	
	struct obj_string* new_string = OBJ(struct obj_string,OBJ_STRING);
	new_string->length = length;
	new_string->chars = chars;
	new_string->hash = hash(chars,length);
	table_set(&vm.strings,new_string,NIL_VAL);
	return new_string;
}

struct obj_string* cpy_str(const char* chars,size_t length){
	uint32_t _hash = hash(chars,length);
	struct obj_string* found_str = table_find_str(&vm.strings,chars,length,_hash);
	if (found_str != NULL) {
		return found_str;
	}
	char* str_heap = f_realloc(NULL,0,length);
	memcpy(str_heap,chars,length);
	str_heap[length] = '\0';
	return alloc_string(str_heap,length);
}


struct obj_string* take_str(char* chars,size_t length){
	uint32_t _hash = hash(chars,length);
	struct obj_string* found_str = table_find_str(&vm.strings,chars,length,_hash);
	if (found_str != NULL) {
		FREE_ARRAY(chars,char,length+1);
		return found_str;
	}
	return alloc_string(chars,length);
}

void print_obj(struct value val) {
	switch (OBJ_TYPE(val)) {
		case OBJ_STRING: printf("%s",AS_CSTRING(val));break;
		default: printf("unknown object");break;
	}
}
