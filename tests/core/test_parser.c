#include "../../lexer.h"
#include "../../chunk.h"
#include "../../parser.h"
#include "../../debug.h"
#include "util.h"
#include <stdbool.h>
#include <stdlib.h>


int main() {
	const char* file = "./test_parser.txt";
	char* source = read_file(file);
	struct chunk ch;
	init_chunk(&ch);
	bool res = compile(&ch,source);
	if (res) {
		disassemble_chunk(&ch,"test chunk");
	}
	free(source);
	free_chunk(&ch);
}
