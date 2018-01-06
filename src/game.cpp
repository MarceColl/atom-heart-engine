#include <stdio.h>

#include <game.hpp>
#include <imgui/imgui.h>
#include <game_introspection.hpp>
#include <debug.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const char* vertex_shader_source =
"#version 400\n"
"layout (location = 0) in vec3 aPos;"
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 projection;"
"void main() {"
"  gl_Position = projection * view * model * vec4(aPos, 1.0);"
"}";

const char* fragment_shader_source =
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

	world_t *world = &state->world;

	entity *first_entity = &world->entities[0];
	first_entity->id = 2;
	first_entity->flags = 0;

	float vertices[] = {
	    -0.5f, -0.5f, 0.0f,
	    0.5f, -0.5f, 0.0f,
	    0.0f,  0.5f, 0.0f
	}; 

	u32 VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	state->vbo = VBO;

	u32 vertex_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	
	u32 fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	u32 shader_program;
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	state->shader_program = shader_program;

	u32 VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	state->vao = VAO;
}

extern "C" GAME_UPDATE_AND_RENDER_FUNC(game_update_and_render) {
	game_state *state;
	state = (game_state*)memory->permanent_storage;

	initialize_arena(&state->world_arena,
			 memory->permanent_storage_space - sizeof(game_state),
			 (u8*)memory->permanent_storage + sizeof(game_state));

	world_t *world = &state->world;
	entity *first_entity = &world->entities[0];

	DEBUG_inspect_struct(6, properties_of_entity, first_entity, "entity");

	glm::mat4 model;
	model = glm::rotate(model, glm::radians(-85.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 view;
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

	u32 modelLoc = glGetUniformLocation(state->shader_program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	u32 viewLoc = glGetUniformLocation(state->shader_program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	u32 projectionLoc = glGetUniformLocation(state->shader_program, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glClearColor(0.0f, 0.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(state->shader_program);
	glBindVertexArray(state->vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	ImGui::ShowDemoWindow();
}
