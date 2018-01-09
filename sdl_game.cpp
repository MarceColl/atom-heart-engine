#include <SDL2/SDL.h>
#include <string>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <GL/gl3w.h>
#include <inttypes.h>
#include <sys/inotify.h>

#include <imgui/imgui_sdl.h>
#include <imgui/imgui.h>
#include <game.hpp>
#include <game_platform.hpp>
#include <memory.hpp>


struct WindowProperties {
    uint16_t width;
    uint16_t height;
    std::string title;
};

char* read_file_to_mem_and_null_terminate(char *filename) {
    char *result = 0;
    FILE *file = fopen(filename, "r");
    if(file) {
	fseek(file, 0, SEEK_END);
	size_t file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	// TODO(Marce): Remove malloc
	result = (char*)malloc(file_size+1);
	fread(result, file_size, 1, file);
	result[file_size] = '\0';

	fclose(file);
    }

    return result;
}

u32 run_command(char *command) {
    // NOTE(Marce): Maybe use fork/exec?
    return system(command);
}

void write_to_log(char *why) {
    
}

SDL_Window* initialize_SDL(WindowProperties winprop) {
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
	fprintf(stderr,"SDL couldn't initialize (%s)\n", SDL_GetError());
	exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);

    SDL_Window *window;
    window = SDL_CreateWindow("Atom Heart Engine",
			      SDL_WINDOWPOS_CENTERED,
			      SDL_WINDOWPOS_CENTERED,
			      1280,
			      720,
			      SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    gl3wInit();

    ImGui_ImplSdlGL3_Init(window);

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
    memory->transient_storage_space = Gigabytes(2);

    uint64_t total_memory =
	memory->permanent_storage_space +
	memory->transient_storage_space;
	
    byte* mem_addr = (byte*)aligned_alloc(128, total_memory);
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
	
    SDL_Window *window = initialize_SDL(winprop);

    game_code code;
    load_game_code(&code);
    int fd = setup_library_modification_notification();

    game_memory memory;
    allocate_game_memory(&memory);

    platform_code platform;
    platform.run_command = &run_command;
    platform.read_file = &read_file_to_mem_and_null_terminate;
    platform.write_to_log = &write_to_log;

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLuint texture;
    glGenTextures(1, &texture);
    printf("%u\n", texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    (*code.initialize_game_state)(&platform, &memory);
	
    bool done = false;
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while(!done) {
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

	SDL_Event event;
	while(SDL_PollEvent(&event)) {
	    ImGui_ImplSdlGL3_ProcessEvent(&event);
	    if(event.type == SDL_QUIT)
		done = true;
	}
	ImGui_ImplSdlGL3_NewFrame(window);

	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui::SetNextWindowSize(ImVec2(815, 650));
	ImGui::Begin("Game", NULL, ImGuiWindowFlags_NoResize); {
	    ImVec2 window_size = ImGui::GetWindowSize();

	    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	    glClear(GL_COLOR_BUFFER_BIT);
	    glViewport(0,0,800,600);
	    (*code.game_update_and_render)(&memory, 1.f);
	    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	    ImGui::GetWindowDrawList()->AddImage(
		(void*)fbo,ImVec2(ImGui::GetCursorScreenPos()),
		ImVec2(ImGui::GetCursorScreenPos().x + 800,
		       ImGui::GetCursorScreenPos().y + 600),
		ImVec2(0, 1),
		ImVec2(1, 0));
	}
	ImGui::End();

	ImGui::Render();
	SDL_GL_SwapWindow(window);
    }
}
