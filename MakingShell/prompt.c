#include<stdio.h>
#include<stdlib.h>
#include "shell.h"

void print_prompt1() {
	fprintf(stderr, "$ ");
}

void print_prompt2() {
	fprintf(stderr,"> ");
}
