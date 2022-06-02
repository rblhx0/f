#ifndef lexer_h
#define lexer_h


#include "common.h"

enum token_type{
	// one char tokens
	TOKEN_COMMA,	  
	TOKEN_DOT,
	TOKEN_EQUAL,
	TOKEN_LEFT_BRACE,
	TOKEN_RIGHT_BRACE,
	TOKEN_LEFT_PARAN,
	TOKEN_RIGHT_PARAN,
	TOKEN_LEFT_BRACKET,
	TOKEN_RIGHT_BRACKET,
	TOKEN_MINUS,
	TOKEN_PLUS,
	TOKEN_STAR,
	TOKEN_BANG,
	TOKEN_SLASH, 
	TOKEN_COLON,
	TOKEN_BACKSLASH,
	// two char tokens
	TOKEN_EQUAL_EQUAL,
	TOKEN_NOT_EQUAL,
	TOKEN_GREATER,
	TOKEN_EQUAL_GREATER,
	TOKEN_LESS,
	TOKEN_EQUAL_LESS,
	// literals
	TOKEN_IDENTIFIER,
	TOKEN_STRING,
	TOKEN_NUMBER,
	// keyword
	TOKEN_IF,
	TOKEN_ELSE,
	TOKEN_ELFE,
	TOKEN_DEF,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_TRUE,
	TOKEN_FALSE,
	TOKEN_FOR,
	TOKEN_WHILE,
	TOKEN_RETURN,
	TOKEN_END,
	TOKEN_DO,
	TOKEN_VAR,
	// builtin functions
	TOKEN_WRITE,
	// special tokens
	TOKEN_EOF,
	TOKEN_NEWLINE,
	TOKEN_ERROR,
};

struct token{
  enum token_type type;
  const char* start;
  int length;
  int line;
};

struct token_array {
	struct token* tokens;
	size_t capacity;
	size_t count;
};

void init_lexer(const char*);
struct token next_token();
void print_token(struct token);
void print_token_array(struct token*);
struct token* get_all_tokens();

#endif
