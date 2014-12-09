#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_HEX_CHARS 2
#define MAX_OCT_CHARS 3

/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */


struct TokenizerT_ {
	char* copied_string;
	char* delimiters;		
	char* current_position;
};

typedef struct TokenizerT_ TokenizerT;


char is_escape_character(char character);
int char_to_hex(char character);
int char_to_oct(char character);
int is_oct_digit(char oct_digit);
char* unescape_string(char* string);
TokenizerT *TKCreate(char *separators, char *ts);
void TKDestroy(TokenizerT *tk);
char is_delimiter(char character, char* delimiters);
char *TKGetNextToken(TokenizerT *tk);
