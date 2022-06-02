#ifndef object_h
#define object_h

#include "common.h"
#include "value.h"

#define OBJ_TYPE(obj) (AS_OBJ(obj)->type)
#define IS_STRING(obj) is_obj_type(obj,OBJ_STRING)

#define AS_STRING(val) ((struct obj_string*)AS_OBJ(val))
#define AS_CSTRING(str) (AS_STRING(str)->chars)

enum obj_type {
	OBJ_STRING,
};

struct object {
	enum obj_type type;
	struct object* next;
};

struct obj_string {
	struct object obj;
	char* chars;
	size_t length;
	uint32_t hash;
};

struct obj_string* cpy_str(const char*,size_t);
struct obj_string* take_str(char* ,size_t );
void print_obj(struct value);

static inline bool is_obj_type(struct value val, enum obj_type type) {
	return IS_OBJ(val) && AS_OBJ(val)->type == type;
}


#endif
