#ifndef _MATERIAL__H_
#define _MATERIAL__H_

#include <game.hpp>
#include <game_math.hpp>

mat_id create_material(char *vertex_filename, char *fragment_filename);
void recompile_material(mat_id material);
void destroy_material(mat_id material);
void use_material(mat_id material);

#endif
