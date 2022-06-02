#ifndef table_h
#define table_h

#include "value.h"
#include "object.h"
#include "common.h"

struct entry {
	struct value v;
	struct obj_string* k;
};

struct table{
	size_t size;
	size_t capacity;
	struct entry* entries;
};


void init_table(struct table*);
void free_table (struct table*);
bool table_del(struct table* ,struct obj_string*);
bool table_set(struct table* ,struct obj_string* ,struct value );
bool table_get(struct table* ,struct obj_string* ,struct value* );
void table_print(struct table*);
struct obj_string* table_find_str(struct table* ,const char* , size_t , uint32_t );

#endif
