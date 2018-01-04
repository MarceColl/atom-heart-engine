#include <string>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
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
	game_update_func *game_update;
	game_render_func *game_render;
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

	code->game_update = (game_update_func*)dlsym(lib_handle, "game_update");
	if((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(1);
	}

	code->game_render = (game_render_func*)dlsym(lib_handle, "game_render");
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
	const struct inotify_event *event;
	ssize_t len;

	len = read(fd, buff, sizeof buff);
	if (len == -1 && errno != EAGAIN) {
		perror("read inotify");
		exit(1);
	}

	return len > 0;
}


void allocate_game_memory(game_memory *memory) {
	memory->permanent_storage_space = Megabytes(64);
	memory->transient_storage_space = Gigabytes(2);

	uint64_t total_memory =
		memory->permanent_storage_space +
		memory->transient_storage_space;
	
	memory->permanent_storage = malloc(total_memory);
}


int main(int argc, char** argv) {
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

	while(!glfwWindowShouldClose(window)) {
		// NOTE(Marce): inotify will tell us when the library
		// has changed, and then we can reload the library.
		// Because we keep the whole game memory as a
		// structure, when the game reloads we will not lose
		// the data. Only in certain cases, like when we
		// change memory layouts, we will have to kill the
		// game and restart it.
		if(code_needs_reload(fd)) {
			load_game_code(&code);
		}

		(*code.game_update)(1.f);
		(*code.game_render)();

		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
