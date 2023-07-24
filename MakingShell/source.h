#ifndef SOURCE_H
#define SOURCE_H

#define EOF		(-1)
#define ERRCHAR 	( 0)
# define INIT_SRC_POS   (-2)

struct source_s {
	char *buffer; //the input text
	long bufsize; //size of the input text
	long curpos; //absolute char position in source (current position)
};

/*
These are functions we are initializing here that we will use in our source.c file
Basically, we are passing into each one a pointer to an instance of the source_s
type we created which has the above qualities/components. So that pointer can include
our current input that we are scanning to parse and the character position of the 
character we are currently scanning is my understanding of why we do this.
*/
char next_char(struct source_s *src); //creating pointer called src to source_s variables we create
void unget_char(struct source_s *src);
char peek_char(struct source_s *src);

