#include "value.h"
#include "memory.h"
#include "object.h"
#include <stdio.h>

void 
init_valuearray(struct value_array* array){
	array->values = NULL;
	array->capacity = 0;
	array->count = 0;
}

void 
write_valuearray(struct value_array* array,struct value val){
	if (array->count + 1 > array->capacity){
		int old_cap = array->capacity;
		array->capacity = GROW_CAP(old_cap);
		array->values   = GROW_ARRAY(array->values,struct value,old_cap,array->capacity);
	}
	array->values[array->count] = val;
	array->count++;
}

void 
free_valuearray(struct value_array* array){
	FREE_ARRAY(array->values,struct value,array->capacity);
	init_valuearray(array);
}

void
print_value(struct value v){
	switch (v.type) {
		case VAL_NUMBER: printf("%g",AS_NUMBER(v));break;
		case VAL_BOOL: printf(AS_BOOL(v) ? "true" : "false");break;
		case VAL_NIL: printf("nil");break;
		case VAL_OBJ: print_obj(v);break;
		default: printf("unknown value");
	}	
}
