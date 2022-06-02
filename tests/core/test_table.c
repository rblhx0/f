#include "../../table.h"
#include "../../object.h"
#include "../../common.h"
#include "../../value.h"
#include "../../memory.h"
#include <stdio.h>

int main() {
	struct table tbl;
	struct obj_string* str1 = cpy_str("one",3);
	struct value val1 = NUMBER_VAL(1);

	struct obj_string* str2 = cpy_str("two",3);
	struct value val2 = NUMBER_VAL(2);

	struct obj_string* str3 = cpy_str("three",5);
	struct value val3 = NUMBER_VAL(3);

	struct obj_string* str4 = cpy_str("yoo",3);
	struct obj_string* val_str = cpy_str("im a value string",17);
	struct value val4 = OBJ_VAL(val_str);

	init_table(&tbl);
	table_set(&tbl,str1,val1);	
	table_set(&tbl,str2,val2);	
	table_set(&tbl,str3,val3);
	table_set(&tbl,str4,val4);
	table_print(&tbl);
	printf("----------------------------------\n");
	table_del(&tbl,str1);
	table_print(&tbl);
	printf("----------------------------------\n");
	table_del(&tbl,str2);
	table_del(&tbl,str3);
	table_print(&tbl);
	printf("----------------------------------\n");
	table_del(&tbl,str4);
	table_print(&tbl);
	printf("%lu\n",tbl.size);
	return 0;
}


