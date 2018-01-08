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
char* allocate_string(string_allocator *sa,
		      u64 size) {
    char *result = sa->first_available_space;
    sa_header *header = (sa_header*)result;
    while((header->magic == MAGIC_HEADER) &&
	  (header->size_available < size)) {
	result = header->next_space;
	header = (sa_header*)result;
    }

    if(header->magic != MAGIC_HEADER) {
	fprintf(stderr, "we dun fucked\n");
    }

    // we want to align the memory to the alignment of the header
    // for faster access, even if we will lose a bit of memory each time
    size = align_to(alignof(sa_header), size);
    sa->used += size;

    if(header->next_space ==  NULL) {
	sa->first_available_space = result + size;
	sa_header* next_header = (sa_header*)sa->first_available_space;
	next_header->size_available = header->size_available - size;
	next_header->next_space = NULL;
	next_header->magic = MAGIC_HEADER;
	fprintf(stderr, "next space null\n");
    }
    else {
	// first_available_space is always on the header of the lowest memory address 
	// TODO 
	sa->first_available_space = result + size;
	sa_header* next_header = (sa_header*)sa->first_available_space;
	next_header->size_available = header->size_available - size;
	next_header->next_space = NULL;
	next_header->magic = MAGIC_HEADER;
    }

    memset(result, size, 0);

    return result;
}

inline
void merge_headers(string_allocator *sa, sa_header *header) {
    sa_header *next_header = (sa_header*)((u64)header + header->size_available);
    printf("next: %x\n", next_header);
    if(next_header->magic == MAGIC_HEADER) {
	printf("MERGE!\n");
    }
}

inline
void free_string(string_allocator *sa, char *str) {
    char *c = str;
    while(*c != '\0') {
	++c;
    }

    sa_header *header = (sa_header*)str;

    u64 size = (u64)c - (u64)str;
    size = align_to(alignof(sa_header), size);
    header->size_available = size;
    header->next_space = sa->first_available_space;
    header->magic = MAGIC_HEADER;

    printf("header: %x\n", header);

    sa->first_available_space = (char*)header;

    merge_headers(sa, header);
}

#endif
