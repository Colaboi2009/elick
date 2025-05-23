#include "node_line_drawer.hpp"

#include "globals.hpp"

void NodeLine::die() {
    if (isInput) {
        if (!from.expired()) {
            from.lock()->removeConnection(fromIndex);
        }
    } else {
        if (!to.expired()) {
            to.lock()->removeConnection(toIndex);
        }
    }
}

void NodeLine::moveAll(float dx, float dy) {
	for (SDL_FPoint &p : drawnPositions) {
		p.x += dx;
		p.y += dy;
	}
}

void NodeLine::render(SDL_FPoint finalPos) {
	if (!complete && !isInput || complete) {
		int index = (isInput ? toIndex : fromIndex);
		bool state = (isInput ? to : from).lock()->state(index);
		sdl.setColor({Uint8(state ? 0 : 255), Uint8(state ? 255 : 0), 0, 255});
	} else {
		sdl.setColor({255, 0, 0, 0});
	}
    SDL_FPoint fromPos = from.lock()->getNodePos(fromIndex, isInput);
    SDL_FPoint toPos;
    if (complete) {
        toPos = to.lock()->getNodePos(toIndex, !isInput);
    } else {
        toPos = finalPos;
    }
    if (drawnPositions.size() == 0) {
        sdl.drawLine(fromPos.x, fromPos.y, toPos.x, toPos.y);
        return;
    }
    for (int i = 0; i < drawnPositions.size() + 2; i++) {
        if (i == 0) {
            sdl.drawLine(fromPos.x, fromPos.y, drawnPositions[0].x, drawnPositions[0].y);
        } else if (i == drawnPositions.size()) {
            sdl.drawLine(drawnPositions[i - 1].x, drawnPositions[i - 1].y, toPos.x, toPos.y);
            break;
        } else {
            sdl.drawLine(drawnPositions[i - 1].x, drawnPositions[i - 1].y, drawnPositions[i].x, drawnPositions[i].y);
        }
    }
}

NodeLineDrawer::NodeLineDrawer() {}

NodeLineDrawer::~NodeLineDrawer() {}

void NodeLineDrawer::tintNodeOnMouse() {
    for (auto g : g_context.getGates()) {
        if (g->inputNodes().size() > 0) {
            for (Node &n : g->inputNodes()) {
                n.c = {0, 0, 255, 255};
            }
        }
        if (g->outputNodes().size() > 0) {
            for (Node &n : g->outputNodes()) {
                n.c = {0, 0, 255, 255};
            }
        }
        if (pointInFRect(g_mousePos, g->renderpos())) {
            Node *n;
            if (g->onNode(g_mousePos, nullptr, nullptr, &n)) {
                n->c = {30, 30, 30, 255};
            }
        }
    }
}

void NodeLineDrawer::predictNodeNearMouse() {
	if (!m_predictNode || !m_draggingNode) {
		return;
	}

	NodeLine &draggedNode = m_nodeLines[m_nodeLines.size() - 1];
	float distance = 9'999'999;
	for (auto g : g_gates) {
		if (g == draggedNode.from.lock()) {
			continue;
		}
		if (draggedNode.isInput) {
			for (int i = 0; i < g->outputNodes().size(); i++) {
				float d = sqrDist(g->outputNodes()[i].p, g_mousePos);
				if (d < distance) {
					distance = d;
					draggedNode.to = g;
					draggedNode.toIndex = i;
				}
			}
		} else {
			for (int i = 0; i < g->inputNodes().size(); i++) {
				if (!g->inGates()[i].expired()) {
					continue;
				}
				float d = sqrDist(g->inputNodes()[i].p, g_mousePos);
				if (d < distance) {
					distance = d;
					draggedNode.to = g;
					draggedNode.toIndex = i;
				}
			}
		}
	}
	if (draggedNode.to.expired()) {
		return;
	}
	if (!draggedNode.isInput) {
		draggedNode.to.lock()->inputNodes()[draggedNode.toIndex].c = {30, 30, 30, 255};
	} else {
		draggedNode.to.lock()->outputNodes()[draggedNode.toIndex].c = {30, 30, 30, 255};
	}
}

void NodeLineDrawer::update(SDL_Event e) {
    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        m_mousePressPos = g_mousePos;
    } else if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        bool detected = false;

		NodeLine &draggedNode = m_nodeLines[m_nodeLines.size() - 1];
		if (m_predictNode) {
			detected = true;
			m_draggingNode = false;
			draggedNode.complete = true;
			if (draggedNode.isInput) {
				draggedNode.from.lock()->connectInput(draggedNode.to, draggedNode.fromIndex, draggedNode.toIndex);
			} else {
				draggedNode.to.lock()->connectInput(draggedNode.from, draggedNode.toIndex, draggedNode.fromIndex);
			}
		} else {
			for (auto g : g_gates) {
				if (!pointInFRect(g_mousePos, g->pos())) {
					continue;
				}

				detected = true;
				bool isInput;
				int index;
				if (m_draggingNode) {
					if (!g->onNode(g_mousePos, &isInput, &draggedNode.toIndex, nullptr) || isInput == draggedNode.isInput) {
						continue;
					}
					draggedNode.to = g;
					m_draggingNode = false;
					draggedNode.complete = true;
					if (draggedNode.isInput) {
						draggedNode.from.lock()->connectInput(draggedNode.to, draggedNode.fromIndex, draggedNode.toIndex);
					} else {
						draggedNode.to.lock()->connectInput(draggedNode.from, draggedNode.toIndex, draggedNode.fromIndex);
					}
				} else if (g->onNode(g_mousePos, &isInput, &index, nullptr)) {
					m_draggingNode = true;
					m_nodeLines.push_back({g, index, isInput});
				}
				break;
			}
		}

        if (!detected && m_draggingNode) {
            m_nodeLines[m_nodeLines.size() - 1].drawnPositions.push_back(m_draggingFinalPos);
        }
        if (pointWithin(m_mousePressPos, g_mousePos, 3)) {
            for (auto g : g_gates) {
                if (!pointInFRect(g_mousePos, g->pos())) {
                    continue;
                }
                auto inp = std::dynamic_pointer_cast<InputGate>(g);
                if (inp) {
                    inp->flip();
                }
            }
        }
    } else if (e.type == SDL_EVENT_KEY_DOWN  && m_draggingNode) {
		if (e.key.key == ck_escape) {
			m_draggingNode = false;
			m_nodeLines.pop_back();
		} else if (e.key.key == ck_predict_node) {
			m_predictNode = true;
		} else if (e.key.key == ck_delete_gate) {
			deleteNodeLineAtCursor();
		}
	} else if (e.type == SDL_EVENT_KEY_UP && e.key.key == ck_predict_node) {
		m_predictNode = false;
	}
	m_draggingFinalPos = g_mousePos;
}

void NodeLineDrawer::render() {
	tintNodeOnMouse();
	predictNodeNearMouse();
    for (int i = m_nodeLines.size() - 1; i >= 0; i--) {
        NodeLine &l = m_nodeLines[i];
        if (l.from.expired() || (l.to.expired() && l.complete)) {
            l.die();
            m_nodeLines.erase(m_nodeLines.begin() + i);
            continue;
        }
        l.render(m_draggingFinalPos);
    }
}

void NodeLineDrawer::moveAll(float dx, float dy) {
	for (auto &n : m_nodeLines) {
		n.moveAll(dx, dy);
	}
}

void NodeLineDrawer::deleteNodeLineAtCursor() {
    constexpr float line_thickness = 5;
    for (int j = 0; j < m_nodeLines.size(); j++) {
        NodeLine &l = m_nodeLines[j];
        bool is = false;
        SDL_FPoint fromPos = l.from.lock()->getNodePos(l.fromIndex, l.isInput);
        SDL_FPoint toPos = l.to.lock()->getNodePos(l.toIndex, !l.isInput);
        if (l.drawnPositions.size() == 0) {
            is = pointOnLine(g_mousePos, fromPos, toPos, line_thickness);
        } else {
            for (int i = 0; i < l.drawnPositions.size() + 2; i++) {
                if (i == 0) {
                    is = pointOnLine(g_mousePos, fromPos, l.drawnPositions[0], line_thickness);
                } else if (i == l.drawnPositions.size()) {
                    is = pointOnLine(g_mousePos, l.drawnPositions[i - 1], toPos, line_thickness);
                } else {
                    is = pointOnLine(g_mousePos, l.drawnPositions[i - 1], l.drawnPositions[i], line_thickness);
                }
                if (is) {
                    break;
                }
            }
        }
        if (is) {
            l.die();
            m_nodeLines.erase(m_nodeLines.begin() + j);
            return;
        }
    }
}

void NodeLineDrawer::straightenLineWithMouseAxis() {
    if (!m_draggingNode) {
        return;
    }

    NodeLine &dragged = m_nodeLines[m_nodeLines.size() - 1];
    SDL_FPoint nodePos = dragged.drawnPositions.size() == 0 ? dragged.from.lock()->getNodePos(dragged.fromIndex, dragged.isInput)
                                                            : dragged.drawnPositions[dragged.drawnPositions.size() - 1];
    if (std::abs(g_mousePos.x - nodePos.x) > std::abs(g_mousePos.y - nodePos.y)) {
        m_draggingFinalPos = {g_mousePos.x, nodePos.y};
    } else {
        m_draggingFinalPos = {nodePos.x, g_mousePos.y};
    }
}
