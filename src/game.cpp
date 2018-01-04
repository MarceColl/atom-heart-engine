#include <stdio.h>

#include <game.hpp>
#include <system.hpp>
#include <render_system.hpp>


extern "C" INITIALIZE_GAME_STATE_FUNC(initialize_game_state) {
	game_state *state;
	state = (game_state*)memory->permanent_storage;

	state->counter = 12;
	state->offset = 14;

	state->update_paused = false;

	state->is_initialized = true;
}


extern "C" GAME_UPDATE_AND_RENDER_FUNC(game_update_and_render) {
	// TODO(Marce): Call all the systems update functions
	game_state *state;
	state = (game_state*)memory->permanent_storage;

	if(not state->update_paused) {
		state->counter++;
		state->offset++;
	}

	render_system rs = render_system();
	rs.update();
}
