#ifndef value_h
#define value_h

#include "common.h"


#define AS_NUMBER(val) ((val).as.number)
#define AS_BOOL(val) ((val).as.boolean)
#define AS_OBJ(val) ((val).as.obj)

#define NUMBER_VAL(n) ((struct value){.type = VAL_NUMBER,{.number = n }})
#define BOOL_VAL(n) ((struct value){.type = VAL_BOOL ,{.boolean = n }})
#define OBJ_VAL(n) ((struct value){.type = VAL_OBJ, {.obj = (struct object*)n}})
#define NIL_VAL ((struct value){.type = VAL_NIL, {.number = 0 }})

#define IS_BOOL(n) ((n).type == VAL_BOOL)
#define IS_NUMBER(n) ((n).type == VAL_NUMBER)
#define IS_NIL(n) ((n).type == VAL_NIL)
#define IS_OBJ(n) ((n).type == VAL_OBJ)

struct object;

enum value_type {
	VAL_NUMBER,
	VAL_BOOL,
	VAL_NIL,
	VAL_OBJ,
};

struct value {
	enum value_type type;
	union {
		bool boolean;
		double number;
		struct object* obj;
	} as ;
};

struct value_array {
	struct value* values;
	int count;
	int capacity;
};

void init_valuearray(struct value_array*);
void free_valuearray(struct value_array*);
void write_valuearray(struct value_array*,struct value);
void print_value(struct value);

#endif
