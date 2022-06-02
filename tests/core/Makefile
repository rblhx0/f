table_files = test_table.c ../../object.c ../../table.c ../../memory.c 
parser_files = test_parser.c ../../object.c ../../memory.c ../../value.c  ../../chunk.c ../../lexer.c ../../parser.c ../../vm.c util.c ../../table.c ../../debug.c
lexer_files = test_lexer.c ../../lexer.c ../../memory.c util.c

table:
	gcc -g $(table_files)

lexer:
	gcc -g $(lexer_files)

parser:
	gcc -g $(parser_files)

