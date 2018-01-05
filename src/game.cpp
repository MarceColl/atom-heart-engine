#include <stdio.h>

#include <game.hpp>
#include <imgui/imgui.h>
#include <game_introspection.hpp>

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

	state->counter = 12;
	state->offset = 14;

	state->update_paused = false;

	state->is_initialized = true;

	state->ent.x = -1;
	state->ent.y = 0;
	
	float points[] = {
	  0.0f, 0.5f, 0.0f,
	  0.5f, -0.5f, 0.0f,
	  -0.5f, -0.5f, 0.0f
	};
	
	glGenBuffers(1, &state->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, state->vbo);
	glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), points, GL_STATIC_DRAW);

	glGenVertexArrays(1, &state->vao);
	glBindVertexArray(state->vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, state->vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs,1,&vertex_shader, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs,1,&fragment_shader, NULL);
	glCompileShader(fs);

	state->shader_program = glCreateProgram();
	glAttachShader(state->shader_program, fs);
	glAttachShader(state->shader_program, vs);
	glLinkProgram(state->shader_program);
}


extern "C" GAME_UPDATE_AND_RENDER_FUNC(game_update_and_render) {
	// TODO(Marce): Call all the systems update functions
	game_state *state;
	state = (game_state*)memory->permanent_storage;

	float x = state->ent.x;
	float y = state->ent.y;

	float points[] = {
	  0.0f+x, 0.5f+y, 0.0f,
	  0.5f+x, -0.5f+y, 0.0f,
	  -0.5f+x, -0.5f+y, 0.0f
	};

	state->ent.x -= 0.0001f;

	glBufferSubData(GL_ARRAY_BUFFER,
			0,
			sizeof(points),
			points);

	glClearColor(0.0f, 0.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(state->shader_program);
	glBindVertexArray(state->vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	ImGui::Begin("test");
	{
		for(int i = 0; i < 2; ++i) {
		    ImGui::Text("%s: %f",
				properties_of_test_entity[i].name,
				*((float*)(&state->ent + properties_of_test_entity[i].offset)));
		}
	}
	ImGui::End();


	if(not state->update_paused) {
		state->counter++;
		state->offset++;
	}
}
