#pragma once

#include <SDL3/SDL_events.h>
#include <vector>
#include "gate.hpp"

class GatePackager {
private:
	enum class Purpose {
		NAME, RED, GREEN, BLUE, 
	};

	std::vector<std::weak_ptr<Gate>> m_toPackage;
	std::string m_name = "";
	SDL_Color m_color = {0, 0, 0, 0};

	std::string m_inputText = "";
	Purpose m_textPurpose;

	bool m_active = false;

	void updateTextInputs();

public:
	GatePackager();
	~GatePackager();

	// begin, render UI, name, color, save, end

	void begin(std::vector<std::weak_ptr<Gate>>);
	void end();

	void update(SDL_Event);
	void render();
	void cancel();
};
