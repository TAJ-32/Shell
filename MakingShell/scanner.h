#ifndef SCANNER_H
#define SCANNER_H

struct token_s {
	struct source_s *src; //pointer to source of input
	int text_len; //length of token text
	char *text; //pointer to char we are looking at within current token
}

//special EOF token, which indicates the end of input
extern struct token_s eof_token;

//tokenize function will ultimately help retrieve the next token from input
struct token_s *tokenize(struct source_s *src); //a function that returns a pointer to a token that accepts the pointer to the entire input as a parameter
void free_token(struct token_s *tok);

#endif
