#include <stdio.h>

#include <game.hpp>
#include <imgui/imgui.h>
#include <game_introspection.hpp>
#include <debug.hpp>

const char* vertex_shader =
"#version 400\n"
"in vec3 vp;"
"void main() {"
"  gl_Position = vec4(vp, 1.0);"
"}";

const char* fragment_shader =
"#version 400\n"
"out vec4 frag_colour;"
"void main() {"
"  frag_colour = vec4(0.5, 0.0, 0.5, 1.0);"
"}";

extern "C" INITIALIZE_GAME_STATE_FUNC(initialize_game_state) {
	game_state *state;
	state = (game_state*)memory->permanent_storage;

	state->update_paused = false;
	state->is_initialized = true;
}


extern "C" GAME_UPDATE_AND_RENDER_FUNC(game_update_and_render) {
	game_state *state;
	state = (game_state*)memory->permanent_storage;

	initialize_arena(&state->world_arena,
			 memory->permanent_storage_space - sizeof(game_state),
			 (u8*)memory->permanent_storage + sizeof(game_state));

	glClearColor(0.0f, 0.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui::ShowDemoWindow();
}
