#pragma once

#include <SDL3/SDL_events.h>
class GatePackager {
private:

public:
	GatePackager();
	~GatePackager();

	// begin, render UI, name, color, save, end

	void beginPackaging();
	void endPackaging();

	void update(SDL_Event);
	void render();
	void cancel();
};
