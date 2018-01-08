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

struct memory_stack {
    u64 size;
    u8 *base;
    u64 used;
};

void initialize_stack(memory_stack *arena,
		      u64 size,
		      u8 *storage);

void *push_struct(memory_stack *arena,
		  u64 size,
		  void *data);

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
