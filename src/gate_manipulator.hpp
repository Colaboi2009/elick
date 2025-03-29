#pragma once

#include <SDL3/SDL_events.h>
class GateManipulator {
private:
public:
	GateManipulator();
	~GateManipulator();

	void update(SDL_Event);
	void render();
};
