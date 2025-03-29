#pragma once

#include <SDL3/SDL_events.h>
class GateAdder {
private:
public:
	GateAdder();
	~GateAdder();

	void update(SDL_Event e);
	void render();
};
