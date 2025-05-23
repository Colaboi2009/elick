#include "selection_box.hpp"

#include "globals.hpp"
#include "helper.hpp"
#include <algorithm>

SelectionBox::SelectionBox() {}

SelectionBox::~SelectionBox() {}

void SelectionBox::update(SDL_Event e) {
    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && SDL_GetMouseState(NULL, NULL) == SDL_BUTTON_LEFT) {
        auto mouseGate = g_context.getGateAt(g_mousePos);

		bool nodeDetected = false;
        if (!mouseGate.expired()) {
			nodeDetected = handleNodeDraggingDetection(mouseGate);
            if (!nodeDetected) {
                handleGateDraggingDetection(mouseGate);
            }
        }
        if (m_draggingNode && !nodeDetected) {
            m_nodeLines.push_back(m_finalNodeDraggingPosition);
        }

        if (!m_dragging && !m_draggingNode) {
            begin();
        }
    } else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        m_dragging = false;
        if (m_selecting) {
            end();
        }
    } else if (e.type == SDL_EVENT_KEY_DOWN && m_selection.size() > 0) {
        switch (e.key.key) {
            case ck_copy: copySelection(); break;
            case ck_paste: pasteSelected(); break;
            case ck_duplicate_gate: duplicateSelection(); break;
            case ck_delete_gate: deleteSelection(); break;
            case ck_package_gates: packageSelection(); break;
            case ck_equalize_horizontal_distance: equalizeDistHor(); break;
            case ck_align_vertically: equalizeDistVer();
            case ck_straighten_selection: straightenSelectionWithSelectionAxis(); break;
            case ck_edit_packaged_gate: editGate(); break;
        }
    }

    if (e.type == SDL_EVENT_KEY_DOWN && m_draggingNode) {
        switch (e.key.key) {
            case ck_escape: cancelNodeDragging(); break;
            case ck_align_node_line_with_axis: alignNodeLineWithAxis(); break;
        }
    } else if (e.type == SDL_EVENT_KEY_UP && m_draggingNode) {
		switch (e.key.key) {
			case ck_align_node_line_with_axis: m_alignNodeLineWithAxis = false; break;
		}
	}
}

void SelectionBox::render() {
    if (m_dragging) {
        moveSelection(g_dmouseX, g_dmouseY);
    }
	
    float w = g_mousePos.x - m_start.x;
    float h = g_mousePos.y - m_start.y;
    SDL_FRect box = {m_start.x + (w > 0 ? 0 : w), m_start.y + (h > 0 ? 0 : h), std::abs(w), std::abs(h)};

    if (m_selecting) {
        sdl.setColor({20, 20, 20, 255});
        sdl.drawRectFilled(box);
        sdl.setColor({50, 50, 50, 255});
        sdl.drawRect(box);
        for (auto g : g_context.getGates()) {
            if (rectsOverlap(box, g->renderpos())) {
                sdl.setColor({60, 60, 60, 255});
            } else {
                sdl.setColor({0, 0, 0, 0});
            }
            sdl.drawRectFilled(g->renderpos());
        }
    } else {
        for (auto g : m_selection) {
            sdl.setColor({60, 60, 60, 255});
            sdl.drawRectFilled(g.lock()->renderpos());
        }
    }

    if (m_draggingNode) {
        renderDraggedNodeLines();
    }
}

bool SelectionBox::handleNodeDraggingDetection(std::weak_ptr<Gate> mouseGate) {
    if (!m_draggingNode) {
        if (auto node = mouseGate.lock()->inputNodeOnPos(g_mousePos).lock()) {
            m_selectedInputNode = node;
            m_draggingNode = true;
			m_finalNodeDraggingPosition = g_mousePos;
			m_nodeLines.push_back(node->p);
            return true;
        } else if (auto node = mouseGate.lock()->outputNodeOnPos(g_mousePos).lock()) {
            m_selectedOutputNode = node;
            m_draggingNode = true;
			m_finalNodeDraggingPosition = g_mousePos;
			m_nodeLines.push_back(node->p);
            return true;
        }
        return false;
    }

    if (m_selectedInputNode.expired()) {
        if (auto inpnode = mouseGate.lock()->inputNodeOnPos(g_mousePos).lock()) {
            auto outnode = m_selectedOutputNode.lock();

            std::reverse(m_nodeLines.begin(), m_nodeLines.end());
            m_nodeLines.pop_back();

            inpnode->owner.lock()->connectInput(outnode, inpnode, m_nodeLines);
            m_selectedOutputNode.reset();
            m_nodeLines.clear();
            m_draggingNode = false;
            return true;
        }
    }
    if (m_selectedOutputNode.expired()) {
        if (auto outnode = mouseGate.lock()->outputNodeOnPos(g_mousePos).lock()) {
            auto inpnode = m_selectedInputNode.lock();

            // watch me commit a crime
            std::reverse(m_nodeLines.begin(), m_nodeLines.end());
            m_nodeLines.pop_back();
            std::reverse(m_nodeLines.begin(), m_nodeLines.end());

            inpnode->owner.lock()->connectInput(outnode, inpnode, m_nodeLines);
            m_nodeLines.clear();
            m_selectedInputNode.reset();
            m_draggingNode = false;
            return true;
        }
    }
    return false;
}

void SelectionBox::handleGateDraggingDetection(std::weak_ptr<Gate> mouseGate) {
    if (!contains(mouseGate)) {
        m_selection.clear();
        m_selection.push_back(mouseGate);
    }
    m_dragging = true;
}

void SelectionBox::renderDraggedNodeLines() {
	if (!m_alignNodeLineWithAxis) {
		m_finalNodeDraggingPosition = g_mousePos;
	} else {
		alignNodeLineWithAxis();
	}

    std::vector<SDL_FPoint> lines{m_nodeLines};

	lines.push_back(m_finalNodeDraggingPosition);

    if (!m_selectedInputNode.expired()) {
        sdl.setColor((m_selectedInputNode.lock()->state() ? SDL_Color(0, 255, 0, 255) : SDL_Color(255, 0, 0, 255)));
    } else if (!m_selectedOutputNode.expired()) {
        sdl.setColor((m_selectedOutputNode.lock()->state ? SDL_Color(0, 255, 0, 255) : SDL_Color(255, 0, 0, 255)));
    }
    sdl.drawLines(lines);
}

void SelectionBox::begin() {
    m_selection.clear();
    m_selecting = true;
    m_start = g_mousePos;
}

void SelectionBox::end() {
    m_selecting = false;

    float w = g_mousePos.x - m_start.x;
    float h = g_mousePos.y - m_start.y;
    SDL_FRect box = {m_start.x + (w > 0 ? 0 : w), m_start.y + (h > 0 ? 0 : h), std::abs(w), std::abs(h)};
    for (auto g : g_context.getGates()) {
        if (rectsOverlap(box, g->renderpos())) {
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
    for (auto g : m_selection) {
        g.lock()->move(dx, dy);
    }
}

void SelectionBox::updateHomeOffset(float dx, float dy) {
	for (int i = 0; i < m_nodeLines.size(); i++) {
		m_nodeLines[i].x += dx;
		m_nodeLines[i].y += dy;
	}
}

void SelectionBox::deleteSelection() {
    for (auto g : m_selection) {
        auto p = std::find(g_context.getGates().begin(), g_context.getGates().end(), g.lock());
        if (p != g_context.getGates().end()) {
            g_context.removeGate(p);
        }
    }
    m_selection.clear();
}

void SelectionBox::copySelection() {}

void SelectionBox::pasteSelected() {}

void SelectionBox::straightenSelectionWithSelectionAxis() {
    float closestDist = 999;
    std::weak_ptr<Gate> closestGate = m_selection[0];
    for (auto g : m_selection) {
        float d = sqrDist(g_mousePos, Point(g.lock()->renderpos()));
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
        Point r0 = g.lock()->renderpos();
        float dx = std::abs(r0.x() - cg->renderpos().x);
        float dy = std::abs(r0.y() - cg->renderpos().y);
        if (dy > dx) {
            g.lock()->pos(cg->realpos().x, g.lock()->realpos().y);
        } else {
            g.lock()->pos(g.lock()->realpos().x, cg->realpos().y);
        }
    }
}

void SelectionBox::equalizeDistHor() {
    constexpr float smallestDist = -3.f;
    std::sort(m_selection.begin(), m_selection.end(),
              [](std::weak_ptr<Gate> l, std::weak_ptr<Gate> r) { return l.lock()->renderpos().x < r.lock()->renderpos().x; });

    for (int i = 0; i < m_selection.size() - 1; i++) {
        auto g0 = m_selection[i].lock();
        auto g1 = m_selection[i + 1].lock();
        g1->pos(g0->realpos().x + smallestDist + g0->realpos().w + (g1->realpos().w * .5f - g0->realpos().w * 0.5f), g0->realpos().y);
    }
}

void SelectionBox::equalizeDistVer() {
    constexpr float smallestDist = -3.f;
    std::sort(m_selection.begin(), m_selection.end(),
              [](std::weak_ptr<Gate> l, std::weak_ptr<Gate> r) { return l.lock()->renderpos().y < r.lock()->renderpos().y; });

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
    // g_uiActive = true;
    std::string name = "CUSTOM" + std::to_string(g_context.gateTypeCount());
    SDL_Color color = {255, 255, 255, 255};
    std::shared_ptr<CustomGate> custom = CustomGate::make(m_selection, g_mousePos, name, color);
	SDL_Log("made the custom gate %s", name.c_str());
    deleteSelection();
	SDL_Log("deleting selection");
    g_context.addNewGate(custom);
	SDL_Log("adding the gate to the possible gates");
    g_context.makeGate(custom->name(), g_mousePos);
	SDL_Log("made a copy to use");
}

void SelectionBox::duplicateSelection() {
    auto orig = m_selection[0].lock();
    auto copy = g_context.makeGate(m_selection[0].lock()->name(), rectToPoint(orig->realpos()));
    SDL_FPoint p = rectToPoint(copy->renderpos());
    while (!g_context.getGateAt(p).expired()) {
        p.y += orig->realpos().h;
    }
    copy->pos(p.x, p.y);
}

void SelectionBox::editGate() {
    if (m_selection.size() != 1) {
        return;
    }
    SDL_Log("Editing gate!");

    std::shared_ptr<CustomGate> gate = std::dynamic_pointer_cast<CustomGate>(m_selection[0].lock());
    if (gate) {
        g_context.startContext(gate->context());
    }
}

void SelectionBox::cancelNodeDragging() {
    m_draggingNode = false;
    m_selectedInputNode.reset();
    m_selectedOutputNode.reset();
    m_nodeLines.clear();
}

void SelectionBox::alignNodeLineWithAxis() {
	m_alignNodeLineWithAxis = true;
	SDL_FPoint nodePos = m_nodeLines[m_nodeLines.size() - 1];
	if (std::abs(g_mousePos.x - nodePos.x) > std::abs(g_mousePos.y - nodePos.y)) {
		m_finalNodeDraggingPosition = {g_mousePos.x, nodePos.y};
	} else {
		m_finalNodeDraggingPosition = {nodePos.x, g_mousePos.y};
	}
}

bool SelectionBox::contains(std::weak_ptr<Gate> g) {
    return std::find_if(m_selection.begin(), m_selection.end(), [g](const std::weak_ptr<Gate> _) { return _.lock() == g.lock(); }) !=
           m_selection.end();
}
