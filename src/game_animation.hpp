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
    frame f;
    char text[50];
} af_token;

struct tokenizer_t {
    char *c;

    void next() {
	++c;
    }
};


af_token get_next_af_token(tokenizer_t *tkzr);

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

bool require_token(tokenizer_t *tkzr, token_type type) {
    af_token tok = get_next_af_token(tkzr);
    bool result = tok.type == type;
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
	af_token frame_num = get_next_af_token(tkzr);
	require_token(tkzr, TOKEN_COMMA);
	af_token transition_time = get_next_af_token(tkzr);
	require_token(tkzr, TOKEN_CBRACKET);
	// TODO(Marce): Error handling
	result.f.index = frame_num.s32val;
	result.f.time = transition_time.f32val;
	result.type = TOKEN_FRAME;
	
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

	    if(0 == strcmp(result.text, "dimensions")) {
		result.type = TOKEN_DIMENSIONS;
	    }
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

#define request_animation_id() \
    request_animation_id_(state)
anim_id request_animation_id_(game_state *state) {
    anim_id result = state->num_animations;
    ++state->num_animations;
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
    u32 frame_counter = 0;
    frame frames[MAX_FRAMES];
    std::string curr_animation;

    tkzr.c = file_contents;
    // NOTE(Marce): Sometimes unordered_map crashes if we don't reserve beforehand
    animator->animations.reserve(1);

    while(parsing) {
	tok = get_next_af_token(&tkzr);
	switch(tok.type) {
	case TOKEN_FRAME: {
	    printf("FOUND TOKEN_FRAME {%d, %f}\n", tok.f.index, tok.f.time);
	    frames[frame_counter].index = tok.f.index;
	    frames[frame_counter].time = tok.f.time;
	    frame_counter++;
	} break;
	case TOKEN_IDENTIFIER: {
	    printf("FOUND TOKEN_IDENTIFIER: %s\n", tok.text);
	    if(frame_counter > 0) {
		anim_id aid = request_animation_id();
		// TODO(Marce): Get rid of malloc
		state->animations[aid].frames = (frame*)malloc(sizeof(frame)*frame_counter);
		memcpy(state->animations[aid].frames, frames, sizeof(frame)*frame_counter);
		state->animations[aid].num_frames = frame_counter;
		state->animations[aid].curr_frame = 0;
		printf("%s: %d\n", curr_animation.c_str(), frame_counter);
		frame_counter = 0;
		printf("%x\n", animator);
		animator->animations.emplace(curr_animation, &state->animations[aid]);
	    }
	    curr_animation = tok.text;
	} break;
	case TOKEN_DIMENSIONS: {
	    af_token x_dim = get_next_af_token(&tkzr);
	    af_token y_dim = get_next_af_token(&tkzr);

	    animator->x_divisions = x_dim.s32val;
	    animator->y_divisions = y_dim.s32val;

	    printf("FOUND TOKEN_DIMENSIONS %d %d\n",
		   animator->x_divisions,
		   animator->y_divisions);
	} break;
	case TOKEN_EOF: {
	    if(frame_counter > 0) {
		anim_id aid = request_animation_id();
		// TODO(Marce): Get rid of malloc
		state->animations[aid].frames = (frame*)malloc(sizeof(frame)*frame_counter);
		memcpy(state->animations[aid].frames, frames, sizeof(frame)*frame_counter);
		state->animations[aid].num_frames = frame_counter;
		state->animations[aid].curr_frame = 0;
		printf("%s: %d\n", curr_animation.c_str(), frame_counter);
		frame_counter = 0;
		animator->animations.emplace(curr_animation, &state->animations[aid]);
	    }
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
