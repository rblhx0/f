#include "../../lexer.h"
#include "util.h"

int main() {
	char* source = read_file("./test_lexer.txt");
	init_lexer(source);
	struct token* ts = get_all_tokens();
	print_token_array(ts);
	free(source);
}
