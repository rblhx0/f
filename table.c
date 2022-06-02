#include "table.h"
#include "object.h"
#include "common.h"
#include "value.h"
#include "memory.h"
#include <stdio.h>
#include <string.h>

#define TABLE_MAX_LOAD 0.75

static struct entry* find_entry(struct entry* entries, struct obj_string* key, size_t capacity){
	uint32_t index = key->hash % capacity;
	struct entry* tombstone = NULL;
	for (;;) {
		struct entry* e = &entries[index];
		if (e->k == NULL) {
			if (IS_NIL(e->v)) {
				// found empty entry
				return tombstone != NULL ? tombstone : e;
			}else {
				if (tombstone == NULL) 
					tombstone = e;
				// fount tombstone
			}
		}else if (e->k == key) {
			return e;
		}
		index = (index+1) % capacity;
	}
}

static void adjust_capacity(struct table* tbl,size_t cap){
	struct entry* entries = ALLOC(struct entry,cap);
	for (size_t i = 0 ; i < cap ; i++) {
		entries[i].k = NULL;
		entries[i].v = NIL_VAL;
	}
	tbl->size = 0;
	for (size_t i = 0 ; i < tbl->capacity;i++){
		struct entry* e = &tbl->entries[i];
		if ( e->k == NULL) {
			continue;
		}
		struct entry* dest = find_entry(entries,e->k,cap);
		dest->k = e->k;
		dest->v = e->v;
		tbl->size++;
	}
	FREE_ARRAY(tbl->entries,struct entry,tbl->capacity);
	tbl->capacity = cap;
	tbl->entries = entries;
}

bool table_set(struct table* tbl,struct obj_string* key,struct value val){
	if (tbl->size + 1 > tbl->capacity * TABLE_MAX_LOAD){
		size_t new_cap = GROW_CAP(tbl->capacity);
		adjust_capacity(tbl,new_cap);
	}
	
	struct entry* e = find_entry(tbl->entries,key,tbl->capacity);
	bool is_key_new = e->k == NULL;
	if (is_key_new && IS_NIL(e->v)) {
		tbl->size++;
	}
	e->k = key;
	e->v = val;
	return is_key_new;
}

bool table_get(struct table* tbl,struct obj_string* key,struct value* val){
	if (tbl->size == 0) {
		return false;
	}
	struct entry* e = find_entry(tbl->entries,key,tbl->capacity);
	if (e->k == NULL) {
		return false;
	}
	*val = e->v;
	return true;
}

bool table_del(struct table* tbl,struct obj_string* key){
	if (tbl->size == 0) {
		return false;
	}
	struct entry* e = find_entry(tbl->entries,key,tbl->capacity);
	if (e->k == NULL) {
		return false;
	}
	e->k = NULL;
	e->v = BOOL_VAL(true);
	return true;
}

struct obj_string* table_find_str(struct table* tbl,const char* str, size_t length, uint32_t hash){
	if (tbl->size == 0) return NULL;

	uint32_t index = hash % tbl->capacity;
	struct entry* tombstone = NULL;
	for (;;) {
		struct entry* e = &tbl->entries[index];
		if (e->k == NULL) {
			if (IS_NIL(e->v)) {
				return NULL;
			}
		}else if (
				e->k->length == length &&
				e->k->hash  == hash &&
				memcmp(str,e->k->chars,length) == 0
			) {
			return e->k;
		}
		index = (index+1) % tbl->capacity;
	}

}

void init_table(struct table* tbl) {
	tbl->capacity = 0;
	tbl->size = 0;
	tbl->entries  = NULL;
}

void free_table (struct table* tbl) {
	FREE_ARRAY(tbl->entries,struct entry,tbl->capacity);
	init_table(tbl);
}

void table_print(struct table* tbl){
	if (tbl->size == 0) {
		printf("empty table\n");
		return;
	}

	for (int i=0 ; i < tbl->capacity; i++) {
		struct entry e = tbl->entries[i];
		if (e.k == NULL) {
			continue;
		}
		printf("{ key: %s",e.k->chars);
		switch(e.v.type) {
			case VAL_NUMBER:
				printf(", value:%g }",AS_NUMBER(e.v));break;
			case VAL_OBJ:
				printf(", value:");print_obj(e.v);printf(" }");break;
			case VAL_BOOL:
				printf(", value:%i }",AS_BOOL(e.v));break;
			case VAL_NIL:
				printf(", value: nil }");break;
			default:
				printf(", value:unknown }");
		}
		printf("\n");
	}
}
