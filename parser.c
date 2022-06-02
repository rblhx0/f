#include "parser.h"
#include "chunk.h"
#include "lexer.h"
#include "common.h"
#include "memory.h"
#include "value.h"
#include "object.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum precedence {
	PREC_NONE,
	PREC_ASSIGN,
	PREC_OR,
	PREC_AND,
	PREC_EQUALITY,
	PREC_COMPARISON,
	PREC_TERM,
	PREC_FACTOR,
	PREC_UNARY,
	PREC_CALL,
	PREC_PRIMARY,
};

struct parser {
	struct token prev;
	struct token current;
	struct token* tokens;
	struct token* token_top;
	bool had_error;
	bool panic_mode;
};

struct local {
	struct token name;
	int depth;
};

struct compiler {
	struct local locals[UINT8_COUNT];
	int local_count;
	int scope_depth;
};

typedef void (*parse_fn)(bool can_assign);

struct parse_rule {
	parse_fn infix_fn;	
	parse_fn prefix_fn;	
	enum precedence prec;
};

struct parser P;
struct chunk* current_chunk;
struct compiler* current = NULL;

static void init_compiler(struct compiler* c){
	c->local_count = 0;
	c->scope_depth = 0;
	current = c;
}

static void expression();

static void binary(bool);
static void unary(bool);
static void number(bool);
static void grouping(bool);
static void string(bool);
static void variable(bool);
static struct parse_rule rules[] = {
	[TOKEN_PLUS] = {binary,unary,PREC_TERM},
	[TOKEN_MINUS] = {binary,unary,PREC_TERM},
	[TOKEN_STAR] = {binary,NULL,PREC_FACTOR},
	[TOKEN_SLASH] = {binary,NULL,PREC_FACTOR},
	[TOKEN_NUMBER] = {NULL,number,PREC_NONE},
	[TOKEN_NEWLINE] = {NULL,NULL,PREC_NONE},
	[TOKEN_ERROR] = {NULL,NULL,PREC_NONE},
	[TOKEN_EQUAL_EQUAL] = {binary,NULL,PREC_EQUALITY},
	[TOKEN_LESS] = {binary,NULL,PREC_COMPARISON},
	[TOKEN_GREATER] = {binary,NULL,PREC_EQUALITY},
	[TOKEN_EQUAL_GREATER] = {binary,NULL,PREC_COMPARISON},
	[TOKEN_EQUAL_LESS] = {binary,NULL,PREC_EQUALITY},
	[TOKEN_BANG] = {NULL,unary,PREC_NONE},
	[TOKEN_LEFT_PARAN]={NULL,grouping,PREC_NONE},
	[TOKEN_STRING] = {NULL,string,PREC_NONE},
	[TOKEN_WRITE] = {NULL,NULL,PREC_NONE},
	[TOKEN_IDENTIFIER] = {NULL,variable,PREC_NONE},
};

static void error(struct token t, const char* msg) {
	if (P.panic_mode) {
		return;
	}
	fprintf(stderr,"[line %d] Error" , t.line);
	if ( t.type == TOKEN_EOF) {
		fprintf(stderr," at end");
	}else if ( t.type != TOKEN_ERROR) {
		fprintf(stderr," at %.*s",t.length,t.start);
	}
	fprintf(stderr," : %s\n",msg);
	P.had_error = true;
	P.panic_mode = true;
}

static void emit_byte(enum op_code code ) {
	write_chunk(current_chunk,code, P.prev.line);
}

static void emit_bytes(enum op_code c1, enum op_code c2 ) {
	write_chunk(current_chunk,c1, P.prev.line);
	write_chunk(current_chunk,c2, P.prev.line);
}

static bool lookup(int t,enum token_type type) {
	if ( (P.token_top+t)->type == type) {
		return true;
	}
	return false;
}

static void advance() {
	P.prev = P.current;		
	P.current = *P.token_top;
	P.token_top++;
	if (P.current.type == TOKEN_ERROR) {
		error(P.current,P.current.start);
	}
}

static bool token_match(enum token_type type) {
	if (P.current.type == type) {
		advance();
		return true;
	}	
	return false;
}

static struct parse_rule get_rule(enum token_type type){
	return rules[type];
}


static void parse(enum precedence prec){
	advance();
	parse_fn prefix_fn = get_rule(P.prev.type).prefix_fn;
	if (prefix_fn == NULL) {
		error(P.prev,"Expect Expression.");
		return;
	}
	
	bool can_assign = prec <= PREC_ASSIGN;
	prefix_fn(can_assign);
	while ( prec <= get_rule(P.current.type).prec){
		advance();
		parse_fn infix_fn = get_rule(P.prev.type).infix_fn;
		infix_fn(can_assign);	
	}
	if ( can_assign && token_match(TOKEN_EQUAL)) {
		error(P.prev,"invalid assignment target");
	}
}

static void binary(bool can_assign) {
	enum token_type type = P.prev.type;
	struct parse_rule rule = get_rule(type);
	parse(rule.prec);
	switch (type){
		case TOKEN_PLUS: emit_byte(OP_ADD) ;return;
		case TOKEN_MINUS:emit_byte(OP_SUBTRACT);return;
		case TOKEN_SLASH:emit_byte(OP_DIVIDE);return;
		case TOKEN_STAR:emit_byte(OP_MULTIPLY);return;
		case TOKEN_EQUAL_EQUAL: emit_byte(OP_EQUAL);return;
		case TOKEN_GREATER: emit_byte(OP_GREATER);return;
		case TOKEN_LESS: emit_byte(OP_LESS);return;
		case TOKEN_EQUAL_GREATER: emit_bytes(OP_LESS,OP_NOT);return;
		case TOKEN_EQUAL_LESS: emit_bytes(OP_GREATER,OP_NOT);return;
		default: return;
	}
}

static void unary(bool can_assign) {
	enum token_type type = P.prev.type;
	parse(PREC_UNARY);
	switch (type) {
		case TOKEN_MINUS : emit_byte(OP_NEGATE);return;
		case TOKEN_BANG : emit_byte(OP_NOT);return;
		default : return;
	}
}



static void consume(enum token_type t,const char* msg) {
	if ( P.current.type == t) {
		advance();
		return;
	}
	error(P.current,msg);
}

static uint8_t make_constant(struct value v){
	int index = add_constant(current_chunk,v);
	if ( index > UINT8_MAX){
		error(P.prev,"too many constant in one chunk");
		return 0;
	}
	return (uint8_t)index;
}

static void emit_constant(struct value v) {
	emit_bytes(OP_CONSTANT,make_constant(v));
}

static void number(bool can_assign) {
	double value = strtod(P.prev.start,NULL);
	emit_constant(NUMBER_VAL(value));
}

static void string(bool can_assign) {
	struct obj_string* str = cpy_str(P.prev.start+1,P.prev.length-2);		
	emit_constant(OBJ_VAL(str));
}

static void expression() {
	parse(PREC_ASSIGN);
}

static void expression_statement() {
	expression();
	emit_byte(OP_POP);
	consume(TOKEN_NEWLINE,"Expect newline at the end");
}

static void grouping(bool can_assign){
	expression();
	consume(TOKEN_RIGHT_PARAN ,"Expect ')'");
}

static void write_stmt(bool can_assign){
	advance();
	grouping(can_assign);	
	consume(TOKEN_NEWLINE,"Expect newline at the end.");
	emit_byte(OP_WRITE);
}

uint8_t identifier_constant(struct token t) {
	return make_constant(OBJ_VAL(
				cpy_str(t.start,t.length)
				));
}

static bool identifier_equal(struct token t1,struct token t2);

static int resolve_local(struct compiler* C,struct token name){
	for (int index = current->local_count-1; index>=0 ; index--) {
		struct local* loc = &current->locals[index];
		if (identifier_equal(name,loc->name)) {
			if (loc->depth == -1) {
				error(P.prev,"Can't read local variable in its own initializer.");
			}
			return index ;
		}
	}
	return -1;
}

static void named_variable(struct token t,bool can_assign){
	uint8_t get_op, set_op;
	int arg = resolve_local(current,t);
	if (arg != -1) {
		get_op = OP_GET_LOCAL;
		set_op = OP_SET_LOCAL;
	}else {
		arg = identifier_constant(t);
		get_op = OP_GET_GLOBAL;
		set_op = OP_SET_GLOBAL;
	}

	if (can_assign && token_match(TOKEN_EQUAL)) {
		expression();
		emit_bytes(set_op,(uint8_t)arg);
		return;
	}
	emit_bytes(get_op,(uint8_t)arg);
}

static void variable(bool can_assign){
	named_variable(P.prev,can_assign);
}

static bool identifier_equal(struct token t1,struct token t2) {
	if (t1.length != t2.length ) return false;	
	return memcmp(t1.start,t2.start,t1.length) == 0;
}

static void add_local(struct token name) {
	if (current->local_count == UINT8_COUNT){
		error(P.prev,"too many local variable in function");
		return;
	}

	struct local* loc = &current->locals[current->local_count++];
	loc->name = name;
	loc->depth = -1;
}

static void declare_variable() {
	if (current->scope_depth == 0) {
		return;
	}
	struct token* name = &P.prev;
	// check if the variable already declared on the same scope
	for (int index = current->local_count-1; index>=0 ; index--) {
		struct local* loc = &current->locals[index];
		if (loc->depth != -1 && loc->depth < current->scope_depth) {
			break;
		}
		if (identifier_equal(*name,loc->name)) {
			error(P.prev,"Identifier has already been declared");
		}
	}
	add_local(*name);
}

static uint8_t parse_variable() {
	consume(TOKEN_IDENTIFIER,"Expect a variable name");
	declare_variable();
	if (current->scope_depth > 0){
		return 0;
	}
	return identifier_constant(P.prev);
}

static void mark_intilized() {
	current->locals[current->local_count - 1].depth = current->scope_depth;	
}
static void define_variable(uint8_t index) {
	if (current->scope_depth > 0) {
		mark_intilized();
		return;
	}
	emit_bytes(OP_DEFINE_GLOBAL,index);
}

static void variable_decl() {
	uint8_t index = parse_variable();
	advance();
	expression();
	consume(TOKEN_NEWLINE,"Expect newline after variable declaration");
	define_variable(index);
}

static void if_statement() {
	expression();
}

static void statement() {
	if (token_match(TOKEN_WRITE)) {
		write_stmt(false);		
	}else if (token_match(TOKEN_IF)){
		if_statement();
	}else{
		expression_statement();		
	}
}

static void declaration();

static void block() {
	while (!lookup(-1,TOKEN_END) && !lookup(-1,TOKEN_EOF)){
		declaration() ;
	}
	consume(TOKEN_END,"Expect 'end' after block.");
}

static void begin_scope() {
	current->scope_depth++;
}

static void end_scope() {
	current->scope_depth--;
	while (current->local_count > 0 &&
			current->locals[current->local_count-1].depth > current->scope_depth
		  ) {
		emit_byte(OP_POP);
		current->local_count--;
	}
}


static void declaration() {
	if (token_match(TOKEN_VAR)) {
		variable_decl();
	}else if(token_match(TOKEN_DO)){
		begin_scope();
		block();
		end_scope();
	}else {
		statement();	
	}
}

static void end(){
	emit_byte(OP_RETURN);
}

static void free_parser(){
	FREE_ARRAY(P.tokens,struct token, 0);
}

bool compile(struct chunk* ch, const char* source){
	init_lexer(source);	
	struct compiler C;
	init_compiler(&C);
	current_chunk = ch;
	P.had_error = false;
	P.panic_mode = false;
	P.tokens = get_all_tokens();
	P.token_top = P.tokens;
	advance();
	while(!token_match(TOKEN_EOF)){
		if (P.had_error) break;
		declaration();
	}
	end();
	return !P.had_error;
}
