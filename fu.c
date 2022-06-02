#include "chunk.h"
#include "debug.h"
#include "lexer.h"
#include "parser.h"
#include "value.h"
#include "vm.h"
#include <stdio.h>


void run_repl() {
  char buffer[1024];
  printf("welcome to f language repl\n");
  for (;;) {
    printf("$ ");
    if (!fgets(buffer, sizeof(buffer), stdin)) {
      printf("\n");
      break;
    }
    interpret(buffer);
  }
}

int main(int argc, char **argv) {
	init_vm();
	if (argc == 1) {
		run_repl();
	} else {
		printf("not quite there yet!\n");
	}
}
