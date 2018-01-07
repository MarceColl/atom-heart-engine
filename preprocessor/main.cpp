#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define internal static

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"

char* read_file_to_mem_and_null_terminate(char *filename) {
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

typedef enum error_type {
    ERROR_INTROSPECT_INVALID_TYPE,
    ERROR_MISSING_OPAREN_AFTER_INTROSPECT,
    ERROR_INVALID_STRUCT_MEMBER,
    ERROR_MISSING_STRUCT_DEFINITION,
    NUM_ERRORS
} error_type;

std::string error_strings[NUM_ERRORS] = {
    [ERROR_INTROSPECT_INVALID_TYPE] =
    "Introspection applied to invalid type. Valid types are: struct.",
    [ERROR_MISSING_OPAREN_AFTER_INTROSPECT] =
    "Missing '(' after INTROSPECT.",
    [ERROR_INVALID_STRUCT_MEMBER] =
    "Invalid struct member in introspectionable struct.",
    [ERROR_MISSING_STRUCT_DEFINITION] =
    "An introspectionable struct needs to be defined. Missing '{'."
};

typedef enum token_type {
    TOKEN_OPAREN,
    TOKEN_CPAREN,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_ASTERISK,
    TOKEN_OBRACKET,
    TOKEN_CBRACKET,
    TOKEN_OBRACE,
    TOKEN_CBRACE,

    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    TOKEN_NUMERIC,

    TOKEN_UNKNOWN,

    TOKEN_EOF
} token_type;

struct token {
    int length;
    char *text;
    token_type type;
};

inline bool
is_eol(char c) {
    bool result = ((c == '\n') || 
		   (c == '\r'));
    return result;
}

struct tokenizer {
    char *at;
    int line;
    int col;
    char *line_start;
    char *filename;

    void next() {
	++col;

	if(is_eol(at[0])) {
	    ++line;
	    col = 0;
	    line_start = at + 1;
	}

	++at;
    }

    void inc(int n) {
	for(int i = 0; i < n; ++i) {
	    next();
	}
    }
};

internal void print_error_line(tokenizer *tkzr, token *tok) {
    char *at = tkzr->line_start;
    while((at[0] != '\n') && (at[0] != '\r')) {
	++at;
    }
    int line_length = at - tkzr->line_start;
    const int indent = 5;

    int indicator_offset = tkzr->col + indent;
    fprintf(stderr, "\n%.*s%.*s\n", indent, "              ",
	    line_length, tkzr->line_start);
    if(tok != NULL) {
	indicator_offset -= tok->length;
	fprintf(stderr, "%*c%.*s\n", indicator_offset, '^',
		tok->length, "~~~~~~~~~~~~~~~~~~~~~~~~");
    }
    else {
	fprintf(stderr, "%*c\n", indicator_offset, '^');
    }
}

internal void report_error(tokenizer *tkzr, token *tok, error_type type) {
    fprintf(stderr, "%s ~ %sERROR%s: %s (line %d col %d)\n",
	    tkzr->filename,
	    KRED,
	    KNRM,
	    error_strings[type].c_str(),
	    tkzr->line,
	    tkzr->col);
    print_error_line(tkzr, tok);
    exit(1);
}

inline bool
is_whitespace(char c) {
    bool result = ((c == ' ') ||
		   (c == '\t') ||
		   (c == '\n') ||
		   (c == '\r'));
    return result;
}
     
internal void
eat_all_whitespace(tokenizer *tkzr) {
    for(;;) {
	if(is_whitespace(tkzr->at[0])) {
	    tkzr->next();
	}
	// Eat comments
	else if((tkzr->at[0] == '/') &&
		(tkzr->at[1] == '/')) {
	    tkzr->inc(2);
	    while((tkzr->at[0] != '\n') &&
		  (tkzr->at[0] != '\r') &&
		  (tkzr->at[0])) {
		tkzr->next();
	    }
	}
	else if((tkzr->at[0] == '/') &&
		(tkzr->at[1] == '*')) {
	    tkzr->inc(2);
	    while(!((tkzr->at[0] == '*') &&
		    (tkzr->at[1] == '/')) &&
		  (tkzr->at[0])) {
		tkzr->next();
	    }
	}
	// Eat preprocessor directives 
	else if(tkzr->at[0] == '#') {
	    tkzr->next();
	    while((tkzr->at[0] != '\n') &&
		  (tkzr->at[0] != '\r') &&
		  (tkzr->at[0])) {
		tkzr->next();
	    }
	}
	else {
	    break;
	}
    }
}

inline bool
is_alpha(char c) {
    bool result = (((c >= 'a') && (c <= 'z')) ||
		   ((c >= 'A') && (c <= 'Z')));
    return result;
}

inline bool
is_numeric(char c) {
    bool result = ((c >= '0') && (c <= '9'));
    return result;
}

inline bool
token_text_is(token tok, char *match) {
    char *at = match;
    for(int i = 0; i < tok.length; ++i, ++at) {
	if((*at == 0) ||
	   (tok.text[i] != *at)) {
	    return false;
	}
    }

    bool result = (*at == 0);
    return result;
}


internal token
get_next_token(tokenizer *tkzr) {
    eat_all_whitespace(tkzr);

    token tok = {};
    tok.length = 1;
    tok.text = tkzr->at;

    switch(tkzr->at[0]) {
    case '\0': {tok.type = TOKEN_EOF; tkzr->next(); } break;
    case '(': {tok.type = TOKEN_OPAREN; tkzr->next();} break;
    case ')': {tok.type = TOKEN_CPAREN; tkzr->next();} break;
    case ':': {tok.type = TOKEN_COLON; tkzr->next(); } break;
    case ';': {tok.type = TOKEN_SEMICOLON; tkzr->next(); } break;
    case '[': {tok.type = TOKEN_OBRACKET; tkzr->next(); } break;
    case ']': {tok.type = TOKEN_CBRACKET; tkzr->next(); } break;
    case '{': {tok.type = TOKEN_OBRACE; tkzr->next(); } break;
    case '}': {tok.type = TOKEN_CBRACE; tkzr->next(); } break;
    case '*': {tok.type = TOKEN_ASTERISK; tkzr->next(); } break;
    case '"': {
	tkzr->next();
	tok.type = TOKEN_STRING;
	tok.text = tkzr->at;
	while(tkzr->at[0] &&
	      tkzr->at[0] != '"') {
	    if((tkzr->at[0] == '\\') &&
	       tkzr->at[1]) {
		tkzr->next();
	    }
	    tkzr->next();
	}
	tok.length = tkzr->at - tok.text;
	if(tkzr->at[0] == '"') {
	    tkzr->next();
	}
    } break;
    default:
    {
	if(is_alpha(tkzr->at[0])) {
	    tok.type = TOKEN_IDENTIFIER;
	    tok.text = tkzr->at;

	    while(is_alpha(tkzr->at[0]) ||
		  is_numeric(tkzr->at[0]) ||
		  tkzr->at[0] == '_' ||
		  ((tkzr->at[0] == ':') &&
		   (tkzr->at[1] == ':'))) {

		if((tkzr->at[0] == ':') &&
		   (tkzr->at[1] == ':')) {
		    tkzr->inc(2);
		}
		else {
		    tkzr->next();
		}
	    }

	    tok.length = tkzr->at - tok.text;
	    for(int i = 0; i < tok.length; ++i) {
		if(tok.text[i] == ':') {
		    tok.text[i] = '_';
		}
	    }
	}
	else if(is_numeric(tkzr->at[0])) {
	    // TODO
	    tkzr->next();
	}
	else {
	    tok.type = TOKEN_UNKNOWN;
	    tkzr->next();
	}
    } break;
    }

    return tok;
}

internal bool
require_token(tokenizer *tkzr, token_type type, token *tok_out) {
    token tok = get_next_token(tkzr);
    if(tok_out != NULL) {
	*tok_out = tok;
    }
    return tok.type == type;
}

internal void 
parse_member(tokenizer *tkzr, token member_type_tok, token struct_name_tok) {
    bool is_pointer = false;
    bool parsing = true;

    while(parsing) {
	token tok = get_next_token(tkzr);

	switch(tok.type) {
	case TOKEN_ASTERISK:
	{
	    is_pointer = true;
	} break;
	case TOKEN_IDENTIFIER:
	{
	    if(is_pointer) {
		printf("        {metatype_p%.*s,\"%.*s\",(uint64_t)&(((%.*s *)0)->%.*s)},\n",
		       member_type_tok.length,
		       member_type_tok.text,
		       tok.length,
		       tok.text,
		       struct_name_tok.length,
		       struct_name_tok.text,
		       tok.length,
		       tok.text);
	    }
	    else {
		printf("        {metatype_%.*s,\"%.*s\",(uint64_t)&(((%.*s *)0)->%.*s)},\n",
		       member_type_tok.length,
		       member_type_tok.text,
		       tok.length,
		       tok.text,
		       struct_name_tok.length,
		       struct_name_tok.text,
		       tok.length,
		       tok.text);
	    }
	} break;
	case TOKEN_OBRACKET:
	{
	    token NUM_ARRAY = get_next_token(tkzr);
	    token LAST_BRACKET = get_next_token(tkzr);
	} break;
	case TOKEN_SEMICOLON:
	case TOKEN_EOF:
	{
	    parsing = false;
	} break;
	default:
	{
	    report_error(tkzr, &tok, ERROR_INVALID_STRUCT_MEMBER);
	} break;
	}
    }
}

struct parsed_struct {
    char *name;
    parsed_struct *next;
    uint8_t num_properties;
};

parsed_struct *first = NULL;

internal void
parse_struct(tokenizer *tkzr) {
    token name_token = get_next_token(tkzr);
    token error_token; 
    if(!require_token(tkzr, TOKEN_OBRACE, &error_token)) {
	report_error(tkzr, &error_token, ERROR_MISSING_STRUCT_DEFINITION);
	exit(1);
    }
    printf("property_entry properties_of_%.*s[] = {\n",
	   name_token.length,
	   name_token.text);

    uint8_t n_prop = 0;
    for(;;) {
	token member_token = get_next_token(tkzr);
	if(member_token.type == TOKEN_CBRACE) {
	    break;
	}
	else {
	    parse_member(tkzr, member_token, name_token);
	    ++n_prop;
	}

    }
    printf("};\n\n\n");

    parsed_struct *prev_first;
    prev_first = first;
    first = (parsed_struct*)malloc(sizeof(parsed_struct));
    first->next = prev_first;
    first->name = (char*)malloc(name_token.length + 1);
    memcpy(first->name, name_token.text, name_token.length);
    first->name[name_token.length] = 0;
    first->num_properties = n_prop;
}

internal void
parse_introspection_params(tokenizer *tkzr) {
    token tok;
    // TODO

    do {
	tok = get_next_token(tkzr);
    } while(tok.type != TOKEN_CPAREN);
}

internal void parse_introspectable(tokenizer *tkzr) {
    token error_token;
    if(require_token(tkzr, TOKEN_OPAREN, &error_token)) {
	parse_introspection_params(tkzr);

	token type_token = get_next_token(tkzr);
	if(token_text_is(type_token, "struct")) {
	    parse_struct(tkzr);
	}
	else {
	    report_error(tkzr, &type_token, ERROR_INTROSPECT_INVALID_TYPE);
	    exit(1);
	}
    }
    else {
	report_error(tkzr, &error_token, ERROR_MISSING_OPAREN_AFTER_INTROSPECT);
	exit(1);
    }
}

int main(int argc, char **argv) {
    if(argc < 2) {
	fprintf(stderr, "usage: ah_pp FILES\n");
	exit(1);
    }
	
    printf("#ifndef _GAME_GENERATED__H_\n");
    printf("#define _GAME_GENERATED__H_\n");
    printf("// Do not modify! File generated using the INTROSPECTION macro\n");
    printf("#include <game_introspection.hpp>\n\n");
    for(int i = 1; i < argc; ++i) {
	char* file_contents = read_file_to_mem_and_null_terminate(argv[i]);

	tokenizer tkzr = {};
	tkzr.line = 1;
	tkzr.at = file_contents;
	tkzr.filename = argv[1];

	bool parsing = true;
	while(parsing) {
	    token tok = get_next_token(&tkzr);
	    switch(tok.type) {
	    default:
	    {
	    } break;
	    case TOKEN_IDENTIFIER:
	    {
		if(token_text_is(tok, "INTROSPECT")) {
		    parse_introspectable(&tkzr);
		}
	    } break;
	    case TOKEN_UNKNOWN:
	    {} break;
	    case TOKEN_EOF:
	    {
		parsing = false;
	    } break;
	    }
	}

	free(file_contents);
    }
    printf("#endif\n");

    printf("#define INTROSPECTION_SWITCH_TYPE_HELPER\\\n");
    parsed_struct *structs;
    structs = first;
    while(structs != NULL) {
	printf("    case metatype_%s:\\\n"
	       "    {\\\n"
	       "        DEBUG_inspect_struct_header(state, %u, properties_of_%s, member_ptr, member->name);\\\n"
	       "    } break; \\\n",
	       structs->name,
	       structs->num_properties,
	       structs->name);
	structs = structs->next;
    }
    printf("\n");

    printf("#define INTROSPECTION_ENUM_TYPE_HELPER\\\n");
    structs = first;
    while(structs != NULL) {
	printf("    metatype_%s,\\\n",
	       structs->name);
	structs = structs->next;
    }
    printf("\n");

    structs = first;
    while(structs != NULL) {
	printf("#define DEBUG_%s(entity)\\\n"
	       "    DEBUG_inspect_struct(state, %d, properties_of_%s, entity, #entity)\\\n\n",
	       structs->name,
	       structs->num_properties,
	       structs->name);
	structs = structs->next;
    }
    printf("\n");

    structs = first;
    while(structs != NULL) {
	printf("#define DEBUG_%s_header(entity, title)\\\n"
	       "    DEBUG_inspect_struct_header(state, %d, properties_of_%s, entity, title)\\\n\n",
	       structs->name,
	       structs->num_properties,
	       structs->name);
	structs = structs->next;
    }
    printf("\n");
}
