#include "lexer.h"
#include "common.h"
#include <endian.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "memory.h"

struct lexer { 
	const char* start;
	const char* current;
	enum token_type last_token;
	int line;
};

struct lexer L;

void
print_token(struct token t){
	if (t.type == TOKEN_NEWLINE){
		printf("newline  --> type : %d \n",t.type);
		return;
	}
	printf("%.*s --> type : %d \n",t.length,t.start,t.type);
};

void 
init_lexer(const char* source){
	L.start = source;	
	L.current = source;	L.last_token = TOKEN_NEWLINE; 
	L.line = 1;
	L.last_token = TOKEN_ERROR;
}

static char 
advance() {
	L.current++;
	return L.current[-1];
}

static struct token 
make_token(enum token_type type){
	struct token t;
	t.start = L.start;
	t.type = type;
	t.length = (int) (L.current - L.start);
	t.line = L.line;
	L.last_token = type;
	return t;
}

static char
peek(){
	return *L.current;
}

static bool 
isatend(){
	return *L.current == '\0';
}

static char
peek_next(){
	if (isatend()){
		return '\0';
	}
	return L.current[1];
}

static void
skip_comment(){
	while (peek() != '\n' || !isatend()){
		advance();
	}
}

static bool newLine_skipable() {
	return L.last_token == TOKEN_NEWLINE ||
		   L.last_token == TOKEN_DO ||
		   L.last_token == TOKEN_END ;
}

static void 
skip_ws(){
	for (;;) {
		char c = peek();
		switch (c) {
		case ' ':
		case '\r':
		case '\t':
			advance();
			break;
		case '\n':
			if (newLine_skipable()) {
				L.line++;
				advance();
				break;
			} else {
				return;
			}
		case '-':
			if (peek_next() == '-'){
				skip_comment();
				break;
			}else{
				return;
			}
		default:
			return;
		}	
	}
}

struct token
error_token(const char* msg){
	struct token t;
	t.start = msg;
	t.line = L.line;
	t.type = TOKEN_ERROR;
	t.length = (int) strlen(msg);
	return t;
}

bool 
f_isalpha(char c){
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

enum token_type
check_kw(int start,int length, char* rest,enum token_type type){
	if ( start + length == L.current - L.start &&
			memcmp( L.start + start , rest , length) == 0){
		return type;
	}

	return TOKEN_IDENTIFIER;
}


enum token_type 
identifier_type(){
	switch(*L.start){
		case 'f':
			if ( L.current - L.start > 1) {
				switch (L.start[1]) {
					case 'a': return check_kw(2,3,"lse",TOKEN_FALSE);
					case 'o': return check_kw(2,1,"r",TOKEN_FOR);
				}
			}
			break;
		case 'e': 
			if ( L.current - L.start > 2 && L.start[1] == 'l'){
				switch (L.start[2]) {
					case 's': return check_kw(3,1,"e",TOKEN_ELSE);
					case 'f': return check_kw(3,1,"e",TOKEN_ELFE);
				}
			}else{
				return check_kw(1,2,"nd",TOKEN_END);
			}
			break;
		case 'i': return check_kw(1,1,"f",TOKEN_IF);
		case 'v': return check_kw(1,2,"ar",TOKEN_VAR);
		case 'r': return check_kw(1,5,"eturn",TOKEN_RETURN);
		case 'a': return check_kw(1,2,"nd",TOKEN_AND);
		case 'o': return check_kw(1,1,"r",TOKEN_OR);
		case 't': return check_kw(1,3,"rue",TOKEN_TRUE);
		case 'w': 
			if (L.current - L.start > 1){
				switch(L.start[1]){
					case 'r': return check_kw(2,3,"ite",TOKEN_WRITE);
					case 'h': return check_kw(2,3,"ile",TOKEN_WHILE);
				}
			}
		case 'd':
			if (L.current - L.start == 2 && L.start[1] == 'o') {
				return TOKEN_DO;
			}
			if (L.current - L.start > 1){
				switch (L.start[1]) {
					case 'e' : return check_kw(2,1,"f",TOKEN_DEF);
				}
			}

	}
	return TOKEN_IDENTIFIER;
}

struct token
identifier(){
	while (f_isalpha(peek()) || isdigit(peek())) {
		advance();
	}
	return make_token(identifier_type());
}

struct token
number(){
	while (isdigit(peek())){
		advance();
	}

	if (peek() == '.' && isdigit(peek_next())){
		advance();
		while(isdigit(peek())) advance();
	}

	return make_token(TOKEN_NUMBER);
}

struct token
string(){
	while (peek() != '"' && peek() != '\n' && !isatend()){
		advance();
	}	

	if (peek() == '\n') {
		return error_token("string terminated by newline");
	}

	if (isatend()) {
		return error_token("unterminated string");
	}

	advance();
	return make_token(TOKEN_STRING);
}

struct token 
next_token(){
	skip_ws();
	L.start = L.current;	
	if (isatend()){
		return make_token(TOKEN_EOF);
	}
	char c = advance();
	
	if (f_isalpha(c)){
		return identifier();
	}
	if (isdigit(c)){
		return number();
	}

	switch (c) {
		case '\\': return make_token(TOKEN_BACKSLASH);
		case '\n':{
			struct token nl = make_token(TOKEN_NEWLINE);
			L.line++;
			return nl;
		}
		case '+': return make_token(TOKEN_PLUS) ;
		case '-': return make_token(TOKEN_MINUS);
		case '*': return make_token(TOKEN_STAR);
		case '/': return make_token(TOKEN_SLASH);
		case '(': return make_token(TOKEN_LEFT_PARAN);
		case ')': return make_token(TOKEN_RIGHT_PARAN);
		case '{': return make_token(TOKEN_LEFT_BRACE);
		case '}': return make_token(TOKEN_RIGHT_BRACE);
		case '.': return make_token(TOKEN_DOT);
		case '=': 
				  if ( peek() == '=') {
					  advance();
					  return make_token(TOKEN_EQUAL_EQUAL);
				  }
				  return make_token(TOKEN_EQUAL);
		case '!': 
				  if ( peek() == '=') {
					  advance();
					  return make_token(TOKEN_NOT_EQUAL);
				  }
				  return make_token(TOKEN_BANG);
		case '[': return make_token(TOKEN_LEFT_BRACKET);
		case ']': return make_token(TOKEN_RIGHT_BRACKET);
		case '>':
				  if (peek() == '=') {
					advance();
					return make_token(TOKEN_EQUAL_GREATER);
				  }
				  return make_token(TOKEN_GREATER);

		case '<':
				  if (peek() == '=') {
					advance();
					return make_token(TOKEN_EQUAL_LESS);
				  }
				  return make_token(TOKEN_LESS);
		case '"':
			return string();
		case ':':
			return make_token(TOKEN_COLON);
		default: return make_token(TOKEN_ERROR);
	}
}

struct token* get_all_tokens() {
	size_t count = 0;
	size_t capacity = 0;
	struct token* tokens = NULL;
	struct token t;
	while (t.type != TOKEN_EOF) {
		t = next_token();
		if (count + 1 > capacity){
			int old_cap = capacity;	
			capacity = GROW_CAP(old_cap);
			tokens = GROW_ARRAY(tokens,struct token,old_cap,capacity);
		}
		tokens[count] = t;
		count++;
	}
	return tokens;
}

void print_token_array(struct token* tokens) {
	int i = 0;
	struct token t = tokens[i];
	while (t.type != TOKEN_EOF){
		print_token(t);
		i++;
		t = tokens[i];
	}
}
