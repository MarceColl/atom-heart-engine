#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <glm/glm.hpp> #include <glm/gtc/matrix_transform.hpp>

#include <game.hpp>
#include <imgui/imgui.h>
#include <game_introspection.hpp>
#include <debug.hpp>
#include <image_utils.hpp>

const char* vertex_shader_source =
    "#version 400\n"
    "layout (location = 0) in vec3 aPos;"
    "layout (location = 1) in vec2 aTexCoord;"
    "uniform mat4 model;"
    "uniform mat4 view;"
    "uniform mat4 projection;"
    "out vec2 TexCoord;"
    "void main() {"
    "  gl_Position = projection * view * model * vec4(aPos, 1.0f);"
    "  TexCoord = aTexCoord;"
    "}";

const char* fragment_shader_source =
    "#version 400\n"
    "out vec4 frag_colour;"
    "in vec2 TexCoord;"
    "uniform sampler2D ourTexture;"
    "void main() {"
    "  frag_colour = texture(ourTexture, TexCoord);"
    "}";


u32 compile_

mat_id create_material(u32 texture, char *vert_filename, char *frag_filename) {
    char *vert_data = platform->read_file(vert_filename);
    char *frag_data = platform->read_file(frag_filename);

    
    u32 fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
	printf("yeah...\n");
	glGetShaderInfoLog(vertex_shader, 1024, NULL, infoLog);
	printf("%s\n", infoLog);
    }

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
}


extern "C" INITIALIZE_GAME_STATE_FUNC(initialize_game_state) {
    game_state *state;
    state = (game_state*)memory->permanent_storage;

    state->update_paused = true;
    state->stepping = false;
    state->is_initialized = true;
    state->platform = platform;

    s32 width, height, nrChannels;
    u8 *data = stbi_load("assets/container.jpg", &width, &height, &nrChannels, 0);

    u32 texture_2d;
    glGenTextures(1, &texture_2d);
    glBindTexture(GL_TEXTURE_2D, texture_2d);
    printf("%u\n", texture_2d);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    state->texture = texture_2d;

    stbi_image_free(data);

    world_t *world = &state->world;
    world->num_entities = 6;
    world->entities_dirty = true;

    srand((unsigned) time(NULL));

    for(int i = 0; i < world->num_entities; ++i) {
	world->entities[i].transform = glm::mat4(1);
	world->entities[i].transform = glm::scale(world->entities[i].transform,
						  glm::vec3(0.2f, 0.2f, 0.2f));
	world->entities[i].transform = glm::translate(world->entities[i].transform,
						      glm::vec3(10.f*(float)rand()/RAND_MAX-5.f,
								10.f*(float)rand()/RAND_MAX-5.f,
								10.f*(float)rand()/RAND_MAX-5.f));
    }

    entity *first_entity = &world->entities[0];
    first_entity->id = 2;
    first_entity->flags = 0;
    first_entity->transform = glm::mat4(1);
    first_entity->transform = glm::scale(first_entity->transform,
					 glm::vec3(0.5f, 0.5f, 0.5f));

    float vertices[] = {
	0.5f, 0.5f, 0.0f, 0.5f, 0.5f,
	0.5f, -0.5f, 0.0f, 0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, 0.0f, 0.0f, 0.5f
    }; 

    u32 indices[] = {
	0, 1, 3,
	1, 2, 3
    };

    s32 success;
    GLchar infoLog[1024];
    u32 vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
	printf("fuck...\n");
	glGetShaderInfoLog(vertex_shader, 1024, NULL, infoLog);
	printf("%s\n", infoLog);
    }
	

    u32 shader_program;
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    state->shader_program = shader_program;

    u32 VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    state->vbo = VBO;
    state->vao = VAO;
}

s32 entity_priority_compare(const void* _e1, const void* _e2) {
    entity* e1 = (entity*)_e1;
    entity* e2 = (entity*)_e2;

    if(e1->priority == e2->priority) {
	return e1->mat.shader_program < e2->mat.shader_program ?
	    -1 : 1;
    }
    return e1->priority < e2->priority ? -1 : 1;
}

void entity_sort(entity *entities, eid num_entities) {
    qsort(entities, num_entities, sizeof(entity), entity_priority_compare);
}

extern "C" GAME_UPDATE_AND_RENDER_FUNC(game_update_and_render) {
    game_state *state;
    state = (game_state*)memory->permanent_storage;

    world_t *world = &state->world;
    entity *first_entity = &world->entities[0];

    if(ImGui::BeginMainMenuBar()) {
	if(ImGui::BeginMenu("File")) {
	    if(ImGui::MenuItem("Exit")) {
		printf("Ayy lemao\n");
	    }
							   
	    ImGui::EndMenu();
	}

	if(ImGui::BeginMenu("Tools")) {
	    if(ImGui::MenuItem("Inspector", "b")) {
		printf("Ayy lemao\n");
	    }
							   
	    ImGui::EndMenu();
	}

	if(ImGui::BeginMenu("Build")) {
	    if(ImGui::MenuItem("Build Project", "b")) {
		(*state->platform->run_command)("tup");
	    }
							   
	    ImGui::EndMenu();
	}

	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 120);
	if(ImGui::Button("Step")) {
	    state->stepping = true;
	}
	ImGui::Checkbox("Pause", &state->update_paused);

	ImGui::EndMainMenuBar();
    }

    DEBUG_entity(first_entity);
    DEBUG_entity(first_entity+1);

    if(!state->update_paused || state->stepping) {
	first_entity->transform = glm::rotate(first_entity->transform,
					      glm::radians(5.0f),
					      glm::vec3(0.0f, 0.0f, 1.0f));
	first_entity->transform = glm::translate(first_entity->transform,
						 glm::vec3(0.1f, 0.0f, 0.0f));

	state->stepping = false;
    }

    glm::mat4 view;
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

    u32 modelLoc = glGetUniformLocation(state->shader_program, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(first_entity->transform));
    u32 viewLoc = glGetUniformLocation(state->shader_program, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    u32 projectionLoc = glGetUniformLocation(state->shader_program, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    if(world->entities_dirty) {
	// TODO(Marce): Look into this
	// entity_sort(world->entities, world->num_entities);
	world->entities_dirty = false;
    }
	
    glBindTexture(GL_TEXTURE_2D, state->texture);
    glBindVertexArray(state->vao);
    glUseProgram(state->shader_program);
    for(int i = 0; i < world->num_entities; i++) {
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(world->entities[i].transform));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

}
