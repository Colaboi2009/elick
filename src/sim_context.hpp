#pragma once

#include "gate.hpp"
#include <unordered_map>
#include <functional>

class SimContext {
private:
	std::unordered_map<std::string, int> m_nameToIndex;
	std::vector<std::shared_ptr<Gate>> m_gateTypes;

	std::vector<std::shared_ptr<Gate>> m_gConGates;
	SDL_FPoint m_gConOffset;

	bool m_isInContext{false};
	std::string m_contextName;

	float m_scrollPos = 10.f;
	float m_scale = 1.f;

	SDL_FPoint m_activeOffset;
	std::vector<std::shared_ptr<Gate>> m_activeGates;


	std::shared_ptr<Gate> gateByName(std::string n) { return m_gateTypes[m_nameToIndex[n]]; }

public:
	SimContext();
	~SimContext();
	
	int gateTypeCount() const { return m_gateTypes.size(); }
	int activeGateCount() const { return m_activeGates.size(); }
	int rawActiveGateCount() const;
	int scale() const { return m_scale; }
	// index of gate TYPES
	int gateNameToIndex(std::string n) { return m_nameToIndex[n]; }
	// index of ACTIVE gates
	int getGateIndex(std::shared_ptr<Gate>);

	std::shared_ptr<Gate> getGateType(int index) const { return m_gateTypes[index]; }
	bool gateTypeWithNameExists(std::string name) const { return m_nameToIndex.contains(name); }

	const std::vector<std::shared_ptr<Gate>> activeGates() const { return m_activeGates; }

	void loadContext(std::vector<std::shared_ptr<Gate>> p) { m_gConGates = m_activeGates = p; }

	void render();
	void renderGateNodeLines();

	int electrify();

	void startContext(std::vector<std::shared_ptr<Gate>>, std::string name);
	void endContext();

	void move(float dx, float dy);
	void resetPosition() { move(-m_activeOffset.x, -m_activeOffset.y); }
	void resetScale() { m_scrollPos = 10.f; }
	void applyScale();
	void scroll(float dy);

	std::weak_ptr<Gate> getGate(int i) { return m_activeGates[i]; }
	void removeGate(auto i) { m_activeGates.erase(i); };
	void removeGate(int i) { m_activeGates.erase(m_activeGates.begin() + i); };
	std::vector<std::shared_ptr<Gate>> &getGates() { return m_activeGates; }

	std::weak_ptr<Gate> getGateAt(SDL_FPoint pos);

	std::shared_ptr<Gate> makeGate(std::string name, SDL_FPoint);
	void addNewGate(std::shared_ptr<Gate>);
	void addNewGate(std::shared_ptr<Gate>, int index);
};
