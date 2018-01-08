#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <game.hpp>
#include <imgui/imgui.h>
#include <game_introspection.hpp>
#include <debug.hpp>
#include <image_utils.hpp>
#include <game_animation.hpp>

#define compile_shader(shader_data, shader_type) \
    compile_shader_(state, shader_data, shader_type)
u32 compile_shader_(game_state *state, char *shader_data, GLenum shader_type) {
    u32 shader_id;
    s32 success;
    GLchar infoLog[1024];
    
    shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &shader_data, NULL);
    glCompileShader(shader_id);

    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    if(!success) {
	printf("yeah...\n");
	glGetShaderInfoLog(shader_id, 1024, NULL, infoLog);
	printf("%s\n", infoLog);
    }

    return shader_id;
}

#define assign_mat_id() assign_mat_id_(state)
mat_id assign_mat_id_(game_state *state) {
    mat_id new_id = state->num_materials;
    state->num_materials++;
    return new_id;
}

#define create_material(texture, vert_filename, frag_filename, name)		\
    create_material_(state, texture, vert_filename, frag_filename, name)
mat_id create_material_(game_state *state, u32 texture,
			char *vert_filename,
			char *frag_filename,
			char *name = NULL) {
    char *vert_data = state->platform->read_file(vert_filename);
    u32 vertex_shader = compile_shader(vert_data, GL_VERTEX_SHADER);
    free(vert_data);

    char *frag_data = state->platform->read_file(frag_filename);
    u32 fragment_shader = compile_shader(frag_data, GL_FRAGMENT_SHADER);
    free(frag_data);

    material mat;
    mat.texture = texture;
    mat.shader_program = glCreateProgram();
    glAttachShader(mat.shader_program, vertex_shader);
    glAttachShader(mat.shader_program, fragment_shader);
    glLinkProgram(mat.shader_program);
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    mat_id new_id = assign_mat_id();

    mat.vert_filename = (char*)malloc(strlen(vert_filename)+1);
    mat.frag_filename = (char*)malloc(strlen(frag_filename)+1);
    strcpy(mat.vert_filename, vert_filename);
    strcpy(mat.frag_filename, frag_filename);

    if(name != NULL) {
	mat.name = (char*)malloc(strlen(name)+1);
	strcpy(mat.name, name);
	mat.name[strlen(name)] = 0;
    }
    else {
	int index = 0;
	char *c = vert_filename;
	// TODO
	// mat.name = (char*)malloc(index);
	// strncpy(mat.name, vert_filename, index-1);
	// mat.name[index] = 0;
    }
    state->materials[new_id] = mat;
    return new_id;
}

#define bind_material(mat, animator) bind_material_(state, mat, animator);
u32 bind_material_(game_state *state, mat_id mat, animator_t *animator) {
    material *m = &state->materials[mat];

#if 0
    u16 xd = animator->x_divisions;
    u16 yd = animator->y_divisions;
    u32 x_cf = animator->current_frame%xd;
    u32 y_cf = animator->current_frame/xd;
    float x_size = 1.0f/(float)xd;
    float y_size = 1.0f/(float)yd;

    float vertices[] = {
	(x_cf)*x_size-1, (y_cf)*y_size-1,
	(x_cf)*x_size-1, (y_cf)*y_size,
	(x_cf)*x_size, (y_cf)*y_size,
	(x_cf)*x_size, (y_cf)*y_size -1 
    }; 

    u32 indices[] = {
	0, 1, 3,
	1, 2, 3
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glBufferSubData(GL_ARRAY_BUFFER, 3*sizeof(float), 2*sizeof(float), &vertices);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
#endif
    
    glUseProgram(m->shader_program);
    glBindTexture(GL_TEXTURE_2D, m->texture);

    return m->shader_program;
}

#define SETUP_MEMORY_SECTION(ptr, mem_type, section_size, allocator_type, initialize) \
    {									\
	u64 size = section_size;					\
	initialize(ptr,size,(void*)((u8*)memory->mem_type + offset));	\
	offset += section_size;						\
    }

#define TRANSIENT transient_storage
#define PERMANENT permanent_storage
    
void setup_memory_space(game_memory *memory, game_state *state) {
    u64 offset = 0;

    SETUP_MEMORY_SECTION(&state->sa_alloc,
			 transient_storage,
			 Megabytes(200),
			 string_allocator,
			 initialize_string_allocator);
}

extern "C" INITIALIZE_GAME_STATE_FUNC(initialize_game_state) {
    game_state *state;
    state = (game_state*)memory->permanent_storage;

    state->update_paused = true;
    state->stepping = false;
    state->is_initialized = true;

    state->open_inspector = true;
    state->open_material_list = false;
    state->open_entity_list = false;
    state->platform = platform;

    setup_memory_space(memory, state);

    char *str1 = allocate_string(&state->sa_alloc, 20);
    char *str2 = allocate_string(&state->sa_alloc, 20);
    char *str3 = allocate_string(&state->sa_alloc, 20);

    strcpy(str1, "hola que tal?");
    strcpy(str2, "jo be i tu?");
    printf("%s\n", str1);
    printf("%s\n", str2);

    free_string(&state->sa_alloc, str2);
    free_string(&state->sa_alloc, str1);

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
    world->num_entities = 1;
    world->entities_dirty = true;
    world->camera.view = glm::mat4(1);
    world->camera.view = glm::translate(world->camera.view, glm::vec3(0.0f, 0.0f, -20.0f));
    world->camera.projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

    srand((unsigned) time(NULL));


    mat_id basic = create_material(texture_2d,
				      "./shaders/basic.vert",
				      "./shaders/basic.frag",
				      "basic");

    entity *first_entity = &world->entities[0];
    first_entity->id = 2;
    first_entity->mat = basic;
    first_entity->flags = 0;
    first_entity->animator.current_frame = 1;
    first_entity->animator.x_divisions = 4;
    first_entity->animator.y_divisions = 4;
    first_entity->transform = glm::mat4(1);
    first_entity->transform = glm::scale(first_entity->transform,
					 glm::vec3(0.5f, 0.5f, 0.5f));

    float vertices[] = {
	0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
	0.5f, -0.5f, 0.0f, 1.0f, 0.75f,
	-0.5f, -0.5f, 0.0f, 0.75f, 0.75f,
	-0.5f, 0.5f, 0.0f, 0.75f, 1.0f
    }; 

    u32 indices[] = {
	0, 1, 3,
	1, 2, 3
    };


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
    state->ebo = EBO;

    load_animation_file("./assets/animation/wood_box.anim", &first_entity->animator);
}

s32 entity_priority_compare(const void* _e1, const void* _e2) {
    entity* e1 = (entity*)_e1;
    entity* e2 = (entity*)_e2;

    if(e1->priority == e2->priority) {
	return e1->mat< e2->mat ? -1 : 1;
    }
    return e1->priority < e2->priority ? -1 : 1;
}

void entity_sort(entity *entities, eid num_entities) {
    qsort(entities, num_entities, sizeof(entity), entity_priority_compare);
}

void entity_list_window(game_state *state) {
    world_t *world = &state->world;
    ImGui::Begin("Entity List");
    {
	char title[10];
	for(int i = 0; i < world->num_entities; i++) {
	    sprintf(title, "%d", i);
	    DEBUG_entity_header(&world->entities[i], title);
	}
    }
    ImGui::End();
}

void material_list_window(game_state *state) {
    ImGui::Begin("Material List");
    {
	static ImGuiTextFilter filter;
	filter.Draw();

	char title[10];
	for(int i = 0; i < state->num_materials; i++) {
	    if(filter.PassFilter(state->materials[i].name)) {
		DEBUG_material_header(&state->materials[i], state->materials[i].name);
	    }
	}
    }
    ImGui::End();
}

void main_menu_bar(game_state *state) {
    if(ImGui::BeginMainMenuBar()) {
	if(ImGui::BeginMenu("File")) {
	    if(ImGui::MenuItem("Exit")) {
		printf("Ayy lemao\n");
	    }
							   
	    ImGui::EndMenu();
	}

	if(ImGui::BeginMenu("Tools")) {
	    ImGui::Checkbox("Inspector", &state->open_inspector);
	    ImGui::Checkbox("Entity List", &state->open_entity_list);
	    ImGui::Checkbox("Material List", &state->open_material_list);
							   
	    ImGui::EndMenu();
	} 
	if(ImGui::BeginMenu("Build")) {
	    if(ImGui::MenuItem("Build Project", "C-b")) {
		(*state->platform->run_command)("tup");
	    }
	    if(ImGui::MenuItem("Reload", "C-r")) {
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
}

void toolbox(game_state *state) {
    if(state->open_entity_list) {
	entity_list_window(state);
    }
    if(state->open_material_list) {
	material_list_window(state);
    }
}

extern "C" GAME_UPDATE_AND_RENDER_FUNC(game_update_and_render) {
    game_state *state;
    state = (game_state*)memory->permanent_storage;

    world_t *world = &state->world;
    entity *first_entity = &world->entities[0];

    main_menu_bar(state);
    toolbox(state);

    DEBUG_entity(first_entity);
    DEBUG_entity(first_entity+1);
    DEBUG_material(&state->materials[0]);

    if(!state->update_paused || state->stepping) {
	first_entity->animator.current_frame++;
	first_entity->transform = glm::rotate(first_entity->transform,
					      glm::radians(5.0f),
					      glm::vec3(0.0f, 0.0f, 1.0f));
	first_entity->transform = glm::translate(first_entity->transform,
						 glm::vec3(0.1f, 0.0f, 0.0f));
    }

    u32 instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(2, 16, GL_FLOAT, GL_FALSE, sizeof(entity),
			  (void*)(&state->world.entities + OFFSET(entity, transform)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2,1);


    glBindVertexArray(state->vao);
    for(int i = 0; i < world->num_entities; i++) {
	#if 0
	entity *e = &world->entities[i];
	u32 shader_program = bind_material(e->mat, &e->animator);

	u32 modelLoc = glGetUniformLocation(shader_program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(world->entities[i].transform));
	u32 viewLoc = glGetUniformLocation(shader_program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(world->camera.view));
	u32 projectionLoc = glGetUniformLocation(shader_program, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(world->camera.projection));
	    
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 50);
	#endif
    }

    state->stepping = false;
}
