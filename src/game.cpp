#include <stdio.h>

#include <game.hpp>

extern "C" GAME_UPDATE_FUNC(game_update) {
	// TODO(Marce): Call all the systems update functions
	printf("Testing zee super updates\n");
}

extern "C" GAME_RENDER_FUNC(game_render) {
	// TODO(Marce): Do the rendering
	printf("Testing render\n");
}
