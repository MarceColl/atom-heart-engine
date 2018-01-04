#include <string>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <GLFW/glfw3.h>
#include <sys/inotify.h>

#include <game.hpp>


struct WindowProperties {
	uint16_t width;
	uint16_t height;
	std::string title;
};


void glfw_error_callback(int error, const char* description) {
	puts(description);
	printf("error code: %d\n", error);
}


GLFWwindow* initialize_GLFW(WindowProperties winprop) {
	if(!glfwInit()) {
		return NULL;
	}

	GLFWwindow *window = NULL; 
	window = glfwCreateWindow(winprop.width,
				  winprop.height,
				  winprop.title.c_str(),
				  NULL, NULL);
	return window;
}

struct game_code {
	game_update_func *game_update_and_render;
	initialize_game_state_func *initialize_game_state;
};

void load_game_code(game_code *code) {
	static void *lib_handle = NULL;

	if(lib_handle != NULL) {
		dlclose(lib_handle);
	}
	
	lib_handle = dlopen("./lib/libgamecode.so", RTLD_LAZY);

	if(!lib_handle) {
		fprintf(stderr, "libgamecode.so: %s\n", dlerror());
		exit(1);
	}

	char *error;

	code->game_update_and_render = (game_update_func*)dlsym(lib_handle, "game_update_and_render");
	if((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(1);
	}

	code->initialize_game_state = (initialize_game_state_func*)dlsym(lib_handle, "initialize_game_state");
	if((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(1);
	}

	printf("game code: loaded\n");
}


int setup_library_modification_notification() {
	// NOTE(Marce): We want non-blocking because we will check
	// every frame if there has been an update
	int fd = inotify_init1(IN_NONBLOCK);
	if(fd == -1) {
		perror("inotify_init");
		exit(1);
	}

	int watch_fd;
	const char filepath[] = "./lib/";

	// NOTE(Marce): We don't care about the watch descriptor as we
	// are only monitoring one file.  When the file is built it
	// generates a IN_MOVED_TO event at the folder level as the
	// last step.
	if((watch_fd = inotify_add_watch(fd,
					 filepath,
					 IN_MOVED_TO)) == -1) {
		perror("Failed setting up inotify\n");
		exit(1);
	}

	return fd;
}


bool code_needs_reload(int fd) {
	char buff[4096]
		__attribute__ ((aligned(__alignof__(struct inotify_event))));
	ssize_t len;

	len = read(fd, buff, sizeof buff);
	if (len == -1 && errno != EAGAIN) {
		perror("read inotify");
		exit(1);
	}

	return len > 0;
}


void allocate_game_memory(game_memory *memory) {
	// NOTE(Marce): We allocate all the memory we need at the
	// start of the game, that way we can reload game code easily,
	// we know we won't get an OUT OF MEM error in the middle of
	// the game, we take into account data locality, we can manage
	// our own memory properly and we avoid the cost of all
	// allocations except this one.
	memory->permanent_storage_space = Megabytes(64);
	memory->transient_storage_space = Megabytes(128);

	uint64_t total_memory =
		memory->permanent_storage_space +
		memory->transient_storage_space;
	
	byte* mem_addr = (byte*)malloc(total_memory);
	if(mem_addr == NULL) {
		perror("Couldn't allocate memory for the game");
		exit(1);
	}

	memory->permanent_storage = mem_addr;
	memory->transient_storage = mem_addr + Megabytes(64);

	// NOTE(Marce): We require memory to be initilized to 0 at the
	// start.
	memset(mem_addr, 0, total_memory);
}


int main() {
	WindowProperties winprop;
	winprop.width = 640;
	winprop.height = 400;
	winprop.title = "My Window";
	
	GLFWwindow *window = initialize_GLFW(winprop);
	glfwMakeContextCurrent(window);

	game_code code;
	load_game_code(&code);
	int fd = setup_library_modification_notification();

	game_memory memory;
	allocate_game_memory(&memory);

	(*code.initialize_game_state)(&memory);
	
	while(!glfwWindowShouldClose(window)) {
		// NOTE(Marce): inotify will tell us when the library
		// has changed, and then we can reload the library.
		// Because we keep the whole game memory as a
		// structure, when the game reloads we will not lose
		// the data. Only in certain cases, like when we
		// change memory layouts, then we will have to kill
		// the game and restart it.
		if(code_needs_reload(fd)) {
			load_game_code(&code);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
		(*code.game_update_and_render)(&memory, 1.f);
	}
}
