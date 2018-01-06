#include <material.hpp>

#include <stdio.h>
#include <GL/gl3w.h>

mat_id create_material(char *vertex_filename,
		       char *fragment_filename) {
    #if 0
    s32 success;
    u32 vertex_shader;
    u32 fragment_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    const char * const* vertex_program = read_file_to_mem_and_null_terminate(vertex_filename);
    if(vertex_program == NULL) {
	fprintf(stderr, "Couldn't read vertex program\n");
    }

    glShaderSource(vertex_shader, 1, vertex_program, NULL);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
	fprintf(stderr, "Couldn't compile vertex program\n");
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    const char * const* fragment_program = read_file_to_mem_and_null_terminate(fragment_filename);
    if(fragment_program == NULL) {
	fprintf(stderr, "Couldn't read fragment program\n");
    }

    glShaderSource(fragment_shader, 1, fragment_program, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
	fprintf(stderr, "Couldn't compile fragment program\n");
    }

    material mat;
    mat.shader_program = glCreateProgram();
    glAttachShader(mat.shader_program, vertex_shader);
    glAttachShader(mat.shader_program, fragment_shader);
    glLinkProgram(mat.shader_program);

    glGetProgramiv(mat.shader_program, GL_LINK_STATUS, &success);
    if(!success) {
	fprintf(stderr, "Couldn't line program\n");
    }
    #endif
}
