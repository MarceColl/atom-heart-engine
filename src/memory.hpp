#ifndef _MEMORY__H_
#define _MEMORY__H_

#include <stdio.h>
#include <string.h>

#include <game_math.hpp>

struct game_memory {
    u64 permanent_storage_space;
    void *permanent_storage;

    u64 transient_storage_space;
    void *transient_storage;

    bool is_initialized;
};

void set_game_memory_ptr(game_memory *memory);

// LOG_ALLOCATOR
struct log_allocator {
    char *base;
    char *curr;
    u64 size;
};

inline
void initialize_log_allocator(log_allocator *la,
			      u64 size,
			      void *storage) {
    la->base = (char*)storage;
    la->curr = (char*)storage;
    la->size = size;
}

enum LOG_TYPE : char {
    DEBUG,
    NOTICE,
    WARNING,
    ERROR
};

#define log_debug(explanation) \
    push_log(&state->la_alloc, DEBUG, "()", explanation)
#define log_notice(explanation) \
    push_log(&state->la_alloc, NOTICE, "()", explanation)
#define log_warning(explanation) \
    push_log(&state->la_alloc, WARNING, "()", explanation)
#define log_error(explanation) \
    push_log(&state->la_alloc, ERROR, "()", explanation)

#define push_log(la, type, from, explanation)				\
    push_log_(la, type, from ": " explanation,		\
	      sizeof(from ": " explanation));
inline
void push_log_(log_allocator *la, LOG_TYPE type, char *str, size_t size_str) {
    *la->curr = (char)type;
    la->curr += 1;
    la->size += 1;
    strncpy(la->curr, str, size_str);
    la->curr += size_str;
    la->size += size_str;
}

// MEMORY STACK
struct memory_stack {
    u64 size;
    u8 *base;
    u64 used;
};

void initialize_stack(memory_stack *arena,
		      u64 size,
		      u8 *storage);

#define push_struct(stack, type) (type*)push_struct_(stack, sizeof(type))
void *push_struct_(memory_stack *arena,
		   u64 size,
		   void *data);

// STRING ALLOCATOR
struct string_allocator {
    u64 size;
    u64 used;
    char *first_available_space;
};

#define MAGIC_HEADER 0xFFF0FEFF

typedef struct string_allocator_header {
    u64 size_available;
    char *next_space;
    char *prev_space;
    u32 magic;
} sa_header;

static 
void initialize_string_allocator(string_allocator *sa,
				 u64 size,
				 void *storage) {
    sa->size = size;
    sa->first_available_space = (char*)storage;
    sa->used = 0;

    sa_header* init_header = (sa_header*)sa->first_available_space;
    init_header->magic = MAGIC_HEADER;
    init_header->size_available = size;
    init_header->next_space = NULL;
    init_header->prev_space = NULL;

    fprintf(stderr, "String allocator initialized\n");
}

static inline
u64 align_to(u64 alignment, u64 address) {
    u64 rem = address % alignment;
    u64 diff = (alignment - rem);

    if(rem != 0) {
	address += diff;
    }

    return address;
}

inline
char* string_allocator_get_fitting_space(string_allocator * sa,
			u64 size) {
    sa_header *it_headers = (sa_header*)sa->first_available_space;

    while(it_headers->size_available < size) {
	it_headers = (sa_header*)it_headers->next_space;
	    
	// There is no space for this string
	if(it_headers == NULL) return NULL;
    } 

    return (char*)it_headers;
}

inline
char* allocate_string(string_allocator *sa,
		      u64 size) {
    // NOTE(Marce): we want to align the memory to the alignment of the header
    // for faster access, even if we will lose a bit of memory each time
    // we have a u32 before the string to store the size of the string
    u64 alloc_size = align_to(alignof(sa_header),
			      size + sizeof(u32));
    sa->used += alloc_size;

    char *mem = string_allocator_get_fitting_space(sa, alloc_size);
    if(mem == NULL) {
	return NULL;
    }

    char *result = mem + sizeof(u32);
    *((u32*)mem) = size;
    sa_header *header = (sa_header*)mem;
    sa_header *next_header;

    if(header->next_space == NULL) {
	// NOTE(Marce): Happens when we append to the end of the header chain
	next_header = (sa_header*)(mem + alloc_size);
	next_header->next_space = NULL;
	next_header->prev_space = header->prev_space;
	next_header->size_available = header->size_available - alloc_size - sizeof(u32);
	next_header->magic = MAGIC_HEADER;
	sa->first_available_space = (char*)next_header;
    } else {
	if(header->prev_space == NULL) {
	    // NOTE(Marce): when it's the first node available, the first
	    // node availble should be the next from this one
	    sa->first_available_space = header->next_space;
	}
	else {
	    // NOTE(Marce): when it's in the middle of the chain
	    ((sa_header*)(header->prev_space))->next_space = header->next_space;
	    ((sa_header*)(header->next_space))->prev_space = header->prev_space;
	}
    }

    memset(result, 0, size);

    return result;
}

inline
void merge_headers(string_allocator *sa, sa_header *header) {
    // TODO(Marce): Merge contiguous headers to avoid fragmenting
    sa_header *next_header = (sa_header*)((u64)header + header->size_available + sizeof(u32));
    if(next_header->magic == MAGIC_HEADER) {
	printf("MERGE!\n");
    }
}

inline
void free_string(string_allocator *sa, char *str) {
    char *real_address = str - sizeof(u32);
    u32 size = *(u32*)real_address;

    sa_header *header = (sa_header*)real_address;
    header->size_available = size;
    header->next_space = sa->first_available_space;
    header->prev_space = NULL;
    header->magic = MAGIC_HEADER;

    sa->first_available_space = (char*)header;

    merge_headers(sa, header);
}


#endif
