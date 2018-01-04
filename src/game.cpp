#include <stdio.h>

#include <game.hpp>

extern "C" GAME_UPDATE_FUNC(game_update) {
	printf("Testing zee super updates\n");
}

extern "C" GAME_RENDER_FUNC(game_render) {
	printf("Testing render\n");
}
