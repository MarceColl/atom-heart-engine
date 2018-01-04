#include <stdio.h>
#include <stdlib.h>


char* ReadEntireFileIntoMemoryAndNullTerminate(char *filename) {
	char *result = 0;
	FILE *file = fopen(filename, "r");
	if(file) {
		fseek(file, 0, SEEK_END);
		size_t file_size = ftell(file);
		fseek(file, 0, SEEK_SET);

		result = (char*)malloc(file_size+1);
		fread(result, file_size, 1, file);
		result[file_size] = '\0';

		fclose(file);
	}

	return result;
}

typedef enum token_type {
	TOKEN_IDENTIFIER,
	TOKEN_OPAREN,
	TOKEN_CPAREN,
	TOKEN_STRING,
	TOKEN_SEMICOLON,
	TOKEN_COLON,
	TOKEN_ASTERISK,
	TOKEN_OBRACKET,
	TOKEN_CBRACKET,
	TOKEN_OBRACE,
	TOKEN_CBRACE,

	TOKEN_NUMERIC,

	TOKEN_UNKNOWN,

	TOKEN_EOF
} token_type;

struct token {
	int length;
	char *text;
	token_type type;
};

struct tokenizer {
	char *at;
};

inline bool
is_whitespace(char c) {
	bool result = ((c == ' ') ||
		       (c == '\t') ||
		       (c == '\n') ||
		       (c == '\r'));
	return result;
}
     
static void
eat_all_whitespace(tokenizer *tkzr) {
	for(;;) {
		if(is_whitespace(tkzr->at[0])) {
			++tkzr->at;
		}
		else if((tkzr->at[0] == '/') &&
			(tkzr->at[1] == '/')) {
			tkzr->at += 2;
			while((tkzr->at[0] != '\n') &&
			      (tkzr->at[0] != '\r') &&
			      (tkzr->at[0])) {
				++tkzr->at;
			}
		}
		else if((tkzr->at[0] == '/') &&
			(tkzr->at[1] == '*')) {
			tkzr->at += 2;
			while(!((tkzr->at[0] == '*') &&
				(tkzr->at[1] == '/')) &&
			      (tkzr->at[0])) {
				++tkzr->at;
			}
		}
		else {
			break;
		}
	}
}

inline bool is_alpha(char c) {
	bool result = (((c >= 'a') && (c <= 'z')) ||
		       ((c >= 'A') && (c <= 'Z')));
	return result;
}

inline bool is_numeric(char c) {
	bool result = ((c >= '0') && (c <= '9'));
	return result;
}

static token
get_next_token(tokenizer *tkzr) {
	eat_all_whitespace(tkzr);

	token tok = {};
	tok.length = 1;
	tok.text = tkzr->at;

	switch(tkzr->at[0]) {
	case '\0': {tok.type = TOKEN_EOF; ++tkzr->at; } break;
	case '(': {tok.type = TOKEN_OPAREN; ++tkzr->at;} break;
	case ')': {tok.type = TOKEN_CPAREN; ++tkzr->at;} break;
	case ':': {tok.type = TOKEN_COLON; ++tkzr->at; } break;
	case ';': {tok.type = TOKEN_SEMICOLON; ++tkzr->at; } break;
	case '[': {tok.type = TOKEN_OBRACKET; ++tkzr->at; } break;
	case ']': {tok.type = TOKEN_CBRACKET; ++tkzr->at; } break;
	case '{': {tok.type = TOKEN_OBRACE; ++tkzr->at; } break;
	case '}': {tok.type = TOKEN_CBRACE; ++tkzr->at; } break;
	case '*': {tok.type = TOKEN_ASTERISK; ++tkzr->at; } break;
	case '"': {
		++tkzr->at;
		tok.type = TOKEN_STRING;
		tok.text = tkzr->at;
		while(tkzr->at[0] &&
		      tkzr->at[0] != '"') {
			if((tkzr->at[0] == '\\') &&
			   tkzr->at[1]) {
				++tkzr->at;
			}
			++tkzr->at;
		}
		tok.length = tkzr->at - tok.text;
		if(tkzr->at[0] == '"') {
			++tkzr->at;
		}
	} break;
	default:
	{
		if(is_alpha(tkzr->at[0])) {
			tok.type = TOKEN_IDENTIFIER;
			tok.text = tkzr->at;
			++tkzr->at;
			while(is_alpha(tkzr->at[0]) ||
			      is_numeric(tkzr->at[0]) ||
			      tkzr->at[0] == '_') {
				++tkzr->at;
			}
			tok.length = tkzr->at - tok.text;
			--tkzr->at;
		}
		else if(is_numeric(tkzr->at[0])) {
			tok.type = TOKEN_NUMERIC;
			tok.text = tkzr->at;
		}
		else {
			tok.type = TOKEN_UNKNOWN;
		}
		++tkzr->at;
	} break;
	}

	return tok;
}

int main(int argc, char **argv) {
	if(argc != 2) {
		fprintf(stderr, "usage: ah_pp FILE\n");
		exit(1);
	}
	
	char* file_contents = ReadEntireFileIntoMemoryAndNullTerminate("src/game.hpp");

	tokenizer tkzr;
	tkzr.at = file_contents;
	printf("%s", file_contents);
	bool parsing = true;
	while(parsing) {
		token tok = get_next_token(&tkzr);
		switch(tok.type) {
		default:
		{
			printf("%d: %.*s\n", tok.type, tok.length, tok.text);
		} break;
		case TOKEN_UNKNOWN:
		{} break;
		case TOKEN_EOF:
		{
			parsing = false;
		} break;
		}
	}
}
