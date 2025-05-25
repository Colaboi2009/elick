#pragma once

#include <SDL3/SDL_events.h>
#include <string>
#include <vector>

class GateAdder {
private:
	std::string m_inputText = "";
	bool m_active = false;

	std::vector<std::string> m_possibleGates;
	int m_selectedIndex;

	void updatePossibleGateList();

public:
	GateAdder();
	~GateAdder();

	void begin();
	// does not have to be called externally; will automatically be called once either gate was added or escape pressed;
	// stops ui and text input
	void end();

	void update(SDL_Event e);
	void render();
};
