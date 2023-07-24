#include <errno.h>
#include "shell.h"
#include "source.h"


void unget_char(struct source_c *src) { //Return the last character we've read back to the input
	if (src -> curpos < 0) { //saying the src pointer we made in source.h will point to the current position of the character. We can do that because curpos is a component of source_c and src is of that type
		return; //return nothing because the curpos is less than 0 so there is no curpos

	}
	src -> curpos--; //going back a char. Specifically, having the value in our pointer mem address decrement.  "Ungetting it"
}

char next_char(struct source_s *src) { //retrieve the next character from input

	if (!src || !src -> buffer) { //if we have no pointer to any kind of input
		errno = ENODATA;
		return ERRCHAR;
	}

	char c1 = 0;
	if (src -> curpos == INIT_SRC_POS) { //if we are at the original place we started, set the index to EOF as defined in source.h
		src -> curpos = -1;
	}
	else {
		c1 = src -> buffer[src -> curpos]; //otherwise, c1 should be the character in the buffer that src is pointing to. Hence, we are looking at the next char each time this is run/
	}

	if (++src->curpos >= src -> bufsize) { //when we reach the last character in the input
		src -> curpos = src -> bufsize; //we are at the end of the string so we no longer need these to be different
		return EOF;
	}

	return src -> buffer[src -> curpos]; //return the current character we are looking at. The "next_char."
}

char peek_char(struct source_s *src) { //peek to check the character without retrieving it. Same as next_char but it doesn't update the source pointer. next_char returns the same input character that peek_char peeked at.

	if (!src || !src -> buffer) {
		errno = ENODATA;
		return ERRCHAR;
	}

	long pos = src -> curpos;

	if (pos == INIT_SRC_POS) {
		pos++; //go to the next character
	}

	if (pos >= src -> bufsize) { //if our position is greater than the length of the input itself, we've gotten to the end
		return EOF;
	}

	return src -> buffer[pos];
}

void skip_white_spaces(struct source_s *src) {
	char c;

	if (!src || !src -> buffer) {
		return;
	}

	while (((c === peek_char(src)) != EOF) && (c == ' ' || c == '\t')) { //while the peeked character past what we are looking at is a whitepsace
		next_char(src);
	}
}
