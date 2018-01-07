#ifndef _GAME_ANIMATION__H_
#define _GAME_ANIMATION__H_

#include <game.hpp>

enum token_type {
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_OBRACKET,
    TOKEN_CBRACKET,
    TOKEN_FLOAT,
    TOKEN_INTEGER,
    TOKEN_COMMA,

    TOKEN_FRAME,
    
    TOKEN_IDENTIFIER,
    TOKEN_DIMENSIONS,

    TOKEN_UNKNOWN,

    TOKEN_EOF
};

typedef struct animation_format_token {
    token_type type;
    f32 f32val;
    s32 s32val;
    s32 text_length;
    char text[50];
} af_token;

struct tokenizer_t {
    char *c;

    void next() {
	++c;
    }
};

bool is_end_of_line(char c) {
    bool result = ((c == '\n') ||
		   (c == '\r'));

    return result;
}

bool is_whitespace(char c) {
    bool result = ((c == ' ') ||
		   (c == '\t') ||
		   is_end_of_line(c));

    return result;
}

void consume_whitespace(tokenizer_t *tkzr) {
    while(is_whitespace(tkzr->c[0])) {
	tkzr->next();
    }

    if(tkzr->c[0] == '#') {
	while(!is_end_of_line(tkzr->c[0])) {
	    tkzr->next();
	}
	tkzr->next();
    }
}

bool is_alpha(char c) {
    bool result = ((c >= 'a' && c <= 'z') ||
		   (c >= 'A' && c <= 'Z'));
    return result;
	
}

bool is_number(char c) {
    bool result = ((c >= '0' && c <= '9'));
    return result;
}

af_token get_next_af_token(tokenizer_t *tkzr) {
    consume_whitespace(tkzr);

    af_token result;
    result.type = TOKEN_UNKNOWN;
    switch(tkzr->c[0]) {
    case ':':
    {
	tkzr->next();
	result.type = TOKEN_COLON;
    } break;
    case ';':
    {
	tkzr->next();
	result.type = TOKEN_SEMICOLON;
    } break;
    case '{':
    {
	tkzr->next();
	result.type = TOKEN_OBRACKET;
    } break;
    case ',':
    {
	tkzr->next();
	result.type = TOKEN_COMMA;
    } break;
    case '}':
    {
	tkzr->next();
	result.type = TOKEN_CBRACKET;
    } break;
    case '\0':
    {
	tkzr->next();
	result.type = TOKEN_EOF;
    } break;
    default:
    {
	if(is_alpha(tkzr->c[0])) {
	    char *start = tkzr->c;
	    tkzr->next();
	    int counter = 1;

	    result.type = TOKEN_IDENTIFIER;
	    while((tkzr->c[0] == '_') ||
		  (is_alpha(tkzr->c[0])) ||
		  (is_number(tkzr->c[0]))) {
		++counter;
		tkzr->next();
	    }

	    strncpy(result.text, start, counter);
	    result.text[counter] = 0;
	    result.text_length = counter;
	}
	else if(is_number(tkzr->c[0])) {
	    char *start = tkzr->c;
	    tkzr->next();
	    int counter = 1;
	    bool is_floating_point = false;

	    while((tkzr->c[0] == '.') ||
		  (is_number(tkzr->c[0]))) {
		++counter;
		if(tkzr->c[0] == '.') {
		    is_floating_point = true;
		}
		tkzr->next();
	    }

	    strncpy(result.text, start, counter);
	    result.text[counter] = 0;
	    if(is_floating_point) {
		result.f32val = atof(result.text);
		result.type = TOKEN_FLOAT;
	    }
	    else {
		result.s32val = atoi(result.text);
		result.type = TOKEN_INTEGER;
	    }
	}
	else {
	    result.type = TOKEN_UNKNOWN;
	    tkzr->next();
	}
    } break;
    }

    return result;
}

#define load_animation_file(filename, animator) \
    load_animation_file_(state, filename, animator)
void load_animation_file_(game_state *state,
			  char *filename,
			  animator_t *animator) {
    // TODO(Marce): Load and parse the file, store it in
    // animator
    char *file_contents = state->platform->read_file(filename);
    if(!file_contents) {
	fprintf(stderr,
		"Couldn't open file %s\n",
		filename);
    }

    tokenizer_t tkzr;
    af_token tok;
    bool parsing = true;
    tkzr.c = file_contents;

    while(parsing) {
	tok = get_next_af_token(&tkzr);
	switch(tok.type) {
	case TOKEN_COLON: {
	    printf("FOUND TOKEN_COLON\n");
	} break;
	case TOKEN_SEMICOLON: {
	    printf("FOUND TOKEN_SEMICOLON\n");
	} break;
	case TOKEN_OBRACKET: {
	    printf("FOUND TOKEN_OBRACKET\n");
	} break;
	case TOKEN_COMMA: {
	    printf("FOUND TOKEN_COMMA\n");
	} break;
	case TOKEN_CBRACKET: {
	    printf("FOUND TOKEN_CBRACKET\n");
	} break;
	case TOKEN_FLOAT: {
	    printf("FOUND TOKEN_FLOAT %f\n", tok.f32val);
	} break;
	case TOKEN_INTEGER: {
	    printf("FOUND TOKEN_INTEGER %d\n", tok.s32val);
	} break;
	case TOKEN_IDENTIFIER: {
	    printf("FOUND TOKEN_IDENTIFIER: %s\n", tok.text);
	} break;
	case TOKEN_DIMENSIONS: {
	    printf("FOUND TOKEN_DIMENSIONS\n");
	} break;
	case TOKEN_EOF: {
	    parsing = false;
	} break;
	default:
	    break;
	}
    }

    free(file_contents);
}

#define store_animation_file(filename, animator) \
    store_animation_file_(state, filename, animator)
void store_animation_file_(game_state *state,
			   char *filename,
			   const animator_t *animator) {
    // TODO(Marce): Serialize the animator and write it
    // in filename
}

void animation_editor() {

}

#endif
