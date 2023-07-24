#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "shell.h"
#include "scanner.h"
#include "source.h"

char *tok_buf = NULL; //pointer to the buffer in which we'll store the current token
int tok_bufsize = 0; //number of bytes we allocate to this buffer
int tok_bufindex = -1; //current buffer index, which tells us where to add the next input char into the buffer

//special token to indicate end of input
struct token_s eof_token =
{
	.text_len = 0,
};
 
void add_to_buf(char c) { //adds a single character to token buffer. Extends buffer, if buffer is full
	tok_buf[tok_bufindex++] = c //next part of the input, c, that is passed to this function will become part of token. The object pointed to by tok_buffer at the tok_bufindex++ will now be c

	if (tok_bufindex >= tok_bufsize) {
		char *tmp = realloc(tok_buf, tok_bufsize*2) //if our token is bigger than we originally allocated memory to, reallocate more memory
		if (!tmp) {
			errno = ENOMEM;
			return;
		}
		tok_buf = tmp; //updating our pointer to our token to match with the newly reallocated space
		tok_bufsize *= 2; //updating bufsize after we updating memory allocation.
	}

	struct token_s *create_token(char *str) { //will return pointer to token. This func takes a string and converts it to a token_s structure. 
		struct token_s *tok = malloc(sizeof(struct token_s)); //creating a new pointer to a token that has enough memroy allocated to it

		if (!tok) {
			return NULL;
		}

		memset(tok, 0, sizeof(struct token_s)); //make every byte in tok pointer hold a 0 at that memory address

		//this below line simply gets the member called text_len from the struct that tok points to and sets that member text_len to the length of the str we passed through
		tok -> text_len = strlen(str); //the text_len value that is stored in tok pointer should equal the length of the string passed through here ie, the length of the token

		char *nstr = (char *)  malloc(tok -> text_len + 1); //allocate space for this new string. We create this to dynamically allocate memory to a new string pointer that will be of the size of the text we need to scan

		if (!nstr) {
			free(tok); //free up the space allocated to tok
			return NULL;
		}

		strcpy(nstr, str); //put the string in the new allocated memory
		tok -> text = nstr; //update the text that tok points at to the new string because this is more memory efficient

		return tok;
	}

	void free_token(struct token_s *tok) {
		if (tok -> text) {
			free(tok -> text); //if there is a pointer to text, free up that space
		}
		free(tok);
	}

	struct token_s *tokenize(struct source_s *src) {  //loops to read input characters one at a time. If after scanning a token, the buffer is not empty, we end the current token and break out of loop
		int endloop = 0;

		if (!src || !src -> buffer || !src -> bufsize) {
			errno = ENODATA;
			return &eof_token;
		}

		if (!tok_buf) {
			tok_bufsize = 1024; //allocate memory for token buffer if not already done
			tok_buf = malloc(tok_bufsize);
			if (!tok_buf) {
				errno = ENOMEM;
				return &eof_token;
			}
		}

		tok_bufindex = 0;
		tok_buf[0] = '\0';//the object at 0th index of our toker buffer pointer is 0

		char nc = next_char(src); //now look at the next character in our src string pointer

		if (nc == ERRCHAR || nc == EOF) {
			return &eof_token;
		}

		do {
			switch(nc) {
				case ' ': //skip white space
				case '\t': //if there is a tab character
					if (tok_bufindex > 0) { //end the loop because we have reached a new token past the tab character, assuming anything has already been input to the command line
						endloop = 1;
					}
					break;
				case '\n':
					if (tok_bufindex > 0) { //assuming user has already put in input, unget the character we are looking at
						unget_char(src);
					}
					else {
						add_to_buf(nc);
					}
					endloop = 1;//if there is a new line, we want to just stop because there will be a new token to look at
					break;

				default:
					add_to_buf(nc); //add the next char to the buffer
					break;
			}
			if (endloop) {
				break;
			}
		} while ((nc = next_char (src)) != EOF); //while the next char isn't end of file

		if (tok_bufindex == 0) {
			return &eof_token;
		}

		if (tok_bufindex >= tok_bufsize) {
			tok_bufindex--;
		}
		tok_buf[tok_bufindex] = '\0';

		struct token_s *tok = create_token(tok_buf);

		if (!tok) {
			fprintf(stderr, "error: failed to allocate buffer: %s\n", strerror(errno));
			return &eof_token;
		}

		tok -> src = src;
		return tok; //after we have our token that we got from create_token(tok_buf) above, we return this token to the caller of this function.
	}
}
