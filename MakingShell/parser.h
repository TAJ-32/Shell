#ifndef PARSER_H
#define PARSER_H

#include "scanner.h" //for struct token_s
#include "source.h"  //for struct source_s


struct node_s *parse_simple_command(struct token_s *tok);

#endif
