#include "selection_box.hpp"

#include "globals.hpp"

void SelectionBox::begin() {
	m_selection.clear();
	m_selecting = true;
	m_start = g_mousePos;
}

void SelectionBox::render() {
	float w = g_mousePos.x - m_start.x;
	float h = g_mousePos.y - m_start.y;
	SDL_FRect box = {m_start.x + (w > 0 ? 0 : w), m_start.y + (h > 0 ? 0 : h), std::abs(w), std::abs(h)};

	if (m_selecting) {
		sdl.setColor({20, 20, 20, 255});
		sdl.drawRectFilled(box);
		sdl.setColor({50, 50, 50, 255});
		sdl.drawRect(box);
		for (auto g : g_gates) {
			if (rectsOverlap(box, g->pos())) {
				sdl.setColor({60, 60, 60, 255});
			} else {
				sdl.setColor({0, 0, 0, 0});
			}
			sdl.drawRectFilled(g->pos());
		}
	} else {
		for (auto g : m_selection) {
			sdl.setColor({60, 60, 60, 255});
			sdl.drawRectFilled(g.lock()->pos());
		}
	}
}

void SelectionBox::end() {
	m_selecting = false;

	float w = g_mousePos.x - m_start.x;
	float h = g_mousePos.y - m_start.y;
	SDL_FRect box = {m_start.x + (w > 0 ? 0 : w), m_start.y + (h > 0 ? 0 : h), std::abs(w), std::abs(h)};
	for (auto g : g_gates) {
		if (rectsOverlap(box, g->pos())) {
			sdl.setColor({60, 60, 60, 255});
			m_selection.push_back(g);
		}
	}
}

void SelectionBox::cancel() {
	m_selecting = false;
	m_selection.clear();
}

void SelectionBox::moveSelection(float dx, float dy) {
	if (m_selection.size() <= 0) {
		return;
	}
	for (auto g : m_selection) {
		g.lock()->move(dx, dy);
	}
}

void SelectionBox::deleteSelection() {
	if (m_selection.size() <= 0) {
		return;
	}
	for (auto g : m_selection) {
		auto p = std::find(g_gates.begin(), g_gates.end(), g.lock());
		if (p != g_gates.end()) {
			g_gates.erase(p);
		}
	}
	m_selection.clear();
}

void SelectionBox::copySelection() {
	if (m_selection.size() <= 0) {
		return;
	}
}

void SelectionBox::pasteSelected() {
}

void SelectionBox::straightenSelectionWithSelectionAxis() {
	if (m_selection.size() <= 0) {
		return;
	}
	float closestDist = 999;
	std::weak_ptr<Gate> closestGate = m_selection[0];
	for (auto g : m_selection) {
		float d = sqrDist(g_mousePos, Point(g.lock()->pos()));
		if (d < closestDist) {
			closestGate = g;
			closestDist = d;
		}
	}

	auto cg = closestGate.lock();
	for (auto g : m_selection) {
		if (g.lock() == cg) {
			continue;
		}
		Point r0 = g.lock()->pos();
		float dx = std::abs(r0.x() - cg->pos().x);
		float dy = std::abs(r0.y() - cg->pos().y);
		if (dy > dx) {
			g.lock()->pos(cg->realpos().x, g.lock()->realpos().y);
		} else {
			g.lock()->pos(g.lock()->realpos().x, cg->realpos().y);
		}
	}
}

void SelectionBox::equalizeDistHor() {
	if (m_selection.size() <= 0) {
		return;
	}
	constexpr float smallestDist = -3.f;
	std::sort(m_selection.begin(), m_selection.end(), [](std::weak_ptr<Gate> l, std::weak_ptr<Gate> r) {
		return l.lock()->pos().x < r.lock()->pos().x;
	});

	for (int i = 0; i < m_selection.size() - 1; i++) {
		auto g0 = m_selection[i].lock();
		auto g1 = m_selection[i + 1].lock();
		g1->pos(g0->realpos().x + smallestDist + g0->realpos().w + (g1->realpos().w * .5f - g0->realpos().w * 0.5f), g0->realpos().y);
	}
}

void SelectionBox::equalizeDistVer() {
	if (m_selection.size() <= 0) {
		return;
	}
	constexpr float smallestDist = -3.f;
	std::sort(m_selection.begin(), m_selection.end(), [](std::weak_ptr<Gate> l, std::weak_ptr<Gate> r) {
		return l.lock()->pos().y < r.lock()->pos().y;
	});

	for (int i = 0; i < m_selection.size() - 1; i++) {
		auto g0 = m_selection[i].lock();
		auto g1 = m_selection[i + 1].lock();
		g1->pos(g0->realpos().x, g0->realpos().y + smallestDist + g0->realpos().h + (g1->realpos().h * .5f - g0->realpos().h * 0.5f));
	}
}

void SelectionBox::addToSelection(std::vector<std::weak_ptr<Gate>> gates) {
	for (auto g : gates) {
		m_selection.push_back(g);
	}
}

void SelectionBox::packageSelection() {
	if (m_selection.size() <= 0) {
		return;
	}
	g_uiActive = true;
	std::string name = "CUSTOM";
	SDL_Color color = {255, 255, 255, 255};
	std::shared_ptr<CustomGate> custom = std::make_shared<CustomGate>(m_selection, SDL_FRect{g_mousePos.x, g_mousePos.y}, name, color);
	deleteSelection();
	g_gates.push_back(custom);
}

void SelectionBox::duplicateSelection() {
	std::shared_ptr<Gate> copy = m_selection[0].lock()->copy();
	copy->resetConnections();
	g_gates.push_back(copy);
	copy->move(10, 10);
}

bool SelectionBox::contains(std::weak_ptr<Gate> g) {
	return std::find_if(m_selection.begin(), m_selection.end(), [g](const std::weak_ptr<Gate> _) { return _.lock() == g.lock(); }) !=
	m_selection.end();
}
