#include "memory.h"
#include <stdlib.h>

void* 
f_realloc(void* p,size_t old_size,size_t new_size){
	if (new_size == 0) {
		free(p);
		return NULL;
	}
	
	void* res = realloc(p,new_size);
 
	if ( res == NULL) {
		exit(69);
	}

	return res;
}
