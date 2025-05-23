#include "sim_context.hpp"

#include "globals.hpp"
#include <algorithm>

SimContext::SimContext() {
	m_nameToIndex = {
		{"I", 0},
		{"O", 1},
		{"and", 2},
	    {"not", 3},
	};
	m_gateTypes = {
		InputGate::make({}),
		OutputGate::make({}),
		AndGate::make({}),
		NotGate::make({}),
	};
}

SimContext::~SimContext() {
}

void SimContext::render() {
	for (auto g : m_activeGates) {
		g->render();
	}
}

void SimContext::renderGateNodeLines() {
	for (auto g : m_activeGates) {
		g->renderNodeLines();
	}
}

int SimContext::electrify() {
	int i;
	for (i = 0; i < 99; i++) {
		bool dirty = false;
		for (auto g : m_activeGates) {
			std::vector<bool> before = g->states();
			g->electrify();
			for (int i = 0; i < before.size(); i++) {
				if (!dirty) {
					dirty = before[i] != g->state(i);
				} else {
					break;
				}
			}
		}
		if (!dirty) {
			break;
		}
	}
	return i;
}

void SimContext::startContext(std::vector<std::shared_ptr<Gate>> gates) {
	m_gConGates = m_activeGates;
	m_activeGates = gates;

	m_gConOffset = m_activeOffset;
	m_activeOffset = {0, 0};

	m_isInContext = true;
}

void SimContext::endContext() {
	if (!m_isInContext) {
		return;
	}
	m_isInContext = false;
	m_activeGates = m_gConGates;
	m_activeOffset = m_gConOffset;
}

void SimContext::move(float dx, float dy) {
    for (auto g : m_activeGates) {
        g->moveAll(dx, dy);
    }
    m_activeOffset.x += dx;
    m_activeOffset.y += dy;
	g_selectionBox.updateHomeOffset(dx, dy);
}

std::weak_ptr<Gate> SimContext::getGateAt(SDL_FPoint pos) {
	std::reverse(m_activeGates.begin(), m_activeGates.end());
	for (auto g : m_activeGates) {
		if (pointInFRect(pos, g->renderpos())) {
			return g;
		}
	}
	return std::weak_ptr<Gate>();
}

void SimContext::applyScale() {
	static float prevScale = m_scale;
	m_scale = m_scrollPos / 10.f;
	sdl.setRenderScale({m_scale, m_scale});
	SDL_GetMouseState(&g_mousePos.x, &g_mousePos.y);
	g_mousePos.x /= m_scale;
	g_mousePos.y /= m_scale;
	if (prevScale != m_scale) {
		g_dmouseX = 0;
		g_dmouseY = 0;
	} else {
		g_dmouseX = g_mousePos.x - g_prevMousePos.x;
		g_dmouseY = g_mousePos.y - g_prevMousePos.y;
	}
	prevScale = m_scale;
}

void SimContext::scroll(float dy) {
	m_scrollPos += m_scale * dy;
	m_scrollPos = std::max(m_scrollPos, 1.f);
	m_scrollPos = std::min(m_scrollPos, 10000.f);

	float predictedScale = std::max(m_scrollPos / 10.f, 0.1f);
	float mousedx = (g_mousePos.x * m_scale + 1920.f / 2.f) * (1.f / predictedScale - 1.f / m_scale);
	float mousedy = (g_mousePos.y * m_scale + 1080.f / 2.f) * (1.f / predictedScale - 1.f / m_scale);

	move(mousedx / 2.f, mousedy / 2.f);
	m_scale = predictedScale;
	sdl.setRenderScale({m_scale, m_scale});
}

std::shared_ptr<Gate> SimContext::makeGate(std::string name, SDL_FPoint p) {
	if (!m_nameToIndex.contains(name)) {
		return nullptr;
	}

	auto gate = gateByName(name)->copy();
	gate->resetConnections();
	gate->pos(p.x, p.y);
	m_activeGates.push_back(gate);
	return gate;
}

void SimContext::addNewGate(std::shared_ptr<Gate> g) {
	if (m_nameToIndex.contains(g->name())) {
		return;
	}

	g = g->copy();
	g->resetConnections();
	m_nameToIndex[g->name()] = m_gateTypes.size();
	m_gateTypes.push_back(g);
}

int SimContext::rawActiveGateCount() const {
	int gateCount = 0;
	for (auto g : m_activeGates) {
		if (const auto cusG = std::dynamic_pointer_cast<CustomGate>(g)) {
			gateCount += cusG->rawGateCount();
		} else {
			gateCount++;
		}
	}
	return gateCount;
}

int SimContext::getGateIndex(std::shared_ptr<Gate> g) {
	for (int i = 0; i < m_activeGates.size(); i++) {
		if (g == m_activeGates[i]) {
			return i;
		}
	}
	return -1;
}
