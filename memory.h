#ifndef memeory_h
#define memeory_h

#include "common.h"

#define GROW_CAP(cap) \
	((cap) < 8 ? 8 : (cap) * 2)
#define FREE_ARRAY(pointer,type,old_count) \
	f_realloc(pointer, sizeof(type) * old_count, 0)
#define GROW_ARRAY(pointer,type,old_count,new_count) \
	(type*)f_realloc(pointer,sizeof(type) * (old_count) , sizeof(type) * (new_count) )

#define ALLOC(type,size) \
	(type*)f_realloc(NULL,0,sizeof(type) * (size))

void* f_realloc(void*,size_t,size_t);

#endif
