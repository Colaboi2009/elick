#include "gate.hpp"

#include "engine/raw_text.hpp"
#include "helper.hpp"

#include "globals.hpp"
#include <algorithm>
#include <cassert>

void Gate::InputNode::render() {
    sdl.setColor(c);
    sdl.drawCircle(Gate::c_node_radius, p.x, p.y);
}

void Gate::InputNode::renderNodeLines() {
    if (connected.expired()) {
        return;
    }

    std::vector<SDL_FPoint> points;
    points.push_back(p);
    points.insert(points.end(), lineNodes.begin(), lineNodes.end());
    points.push_back(connected.lock()->p);

    sdl.setColor((state() ? SDL_Color(0, 255, 0, 255) : SDL_Color(255, 0, 0, 255)));
    sdl.drawLines(points, 3);
}

void Gate::InputNode::setPos(float x, float y) {
    SDL_FPoint delta = {x - p.x, y - p.y};
    for (auto &l : lineNodes) {
        l.x += delta.x;
        l.y += delta.y;
    }
    p = {x, y};
}

void Gate::InputNode::move(float dx, float dy) {
    p.x += dx;
    p.y += dy;
}

void Gate::InputNode::moveAll(float dx, float dy) {
    p.x += dx;
    p.y += dy;
    for (auto &l : lineNodes) {
        l.x += dx;
        l.y += dy;
    }
}

void Gate::OutputNode::render() {
    sdl.setColor(c);
    sdl.drawCircle(Gate::c_node_radius, p.x, p.y);
}

Gate::Gate(SDL_FPoint pos, int maxIn, int maxOut, std::string name, SDL_Color c)
    : m_maxInput(maxIn), m_maxOutput(maxOut), m_name{name}, m_innerColor{c} {
    m_pos.x = pos.x;
    m_pos.y = pos.y;
}

std::shared_ptr<Gate> Gate::init() {
    resetConnections();
    return shared_from_this();
}

Gate::Gate(const Gate &other) {
    m_pos = other.m_pos;
    m_inner = other.m_inner;
    m_innerColor = other.m_innerColor;
    m_name = other.m_name;

    m_maxInput = other.m_maxInput;
    m_maxOutput = other.m_maxOutput;
    m_inputNodes = other.m_inputNodes;
    m_outputNodes = other.m_outputNodes;

    m_spriteDirty = true;
}

Gate::~Gate() {}

void Gate::move(float dx, float dy) {
    m_pos.x += dx;
    m_pos.y += dy;
    for (auto n : m_inputNodes) {
        n->move(dx, dy);
    }
    for (auto n : m_outputNodes) {
        n->move(dx, dy);
    }
}

void Gate::moveAll(float dx, float dy) {
    m_pos.x += dx;
    m_pos.y += dy;
    for (auto n : m_inputNodes) {
        n->moveAll(dx, dy);
    }
    for (auto n : m_outputNodes) {
        n->move(dx, dy);
    }
}

void Gate::pos(float x, float y) {
    m_pos.x = x;
    m_pos.y = y;
    for (auto n : m_inputNodes) {
        n->setPos(x, y);
    }
    for (auto n : m_outputNodes) {
        n->setPos(x, y);
    }
}

void Gate::connectInput(std::weak_ptr<OutputNode> on, std::weak_ptr<InputNode> in, std::vector<SDL_FPoint> line) {
    on.lock()->toGate = shared_from_this();
    in.lock()->connected = on;
    in.lock()->lineNodes = line;
}

void Gate::resetConnections() {
    m_inputNodes.clear();
    m_outputNodes.clear();
    m_outputNodes.resize(m_maxOutput);
    m_inputNodes.resize(m_maxInput);
    for (int i = 0; i < m_maxInput; i++) {
        m_inputNodes[i] = std::make_shared<InputNode>();
        m_inputNodes[i]->owner = shared_from_this();
    }
    for (int i = 0; i < m_maxOutput; i++) {
        m_outputNodes[i] = std::make_shared<OutputNode>();
        m_outputNodes[i]->owner = shared_from_this();
    }
}

std::weak_ptr<Gate::InputNode> Gate::inputNodeOnPos(SDL_FPoint p) {
    for (auto n : m_inputNodes) {
        if (pointInCircle(p, c_node_radius, n->p)) {
            return n;
        }
    }
    return std::weak_ptr<InputNode>();
}

std::weak_ptr<Gate::OutputNode> Gate::outputNodeOnPos(SDL_FPoint p) {
    for (auto n : m_outputNodes) {
        if (pointInCircle(p, c_node_radius, n->p)) {
            return n;
        }
    }
    return std::weak_ptr<OutputNode>();
}

int Gate::getInputNodeIndex(std::weak_ptr<InputNode> n) {
	auto node = n.lock();
	for (int i = 0; i < m_inputNodes.size(); i++) {
		if (m_inputNodes[i] == node) {
			return i;
		}
	}
	return -1;
}

int Gate::getOutputNodeIndex(std::weak_ptr<OutputNode> n) {
	auto node = n.lock();
	for (int i = 0; i < m_outputNodes.size(); i++) {
		if (m_outputNodes[i] == node) {
			return i;
		}
	}
	return -1;
}

void Gate::render() {
    if (m_spriteDirty) {
        createSprite();
        m_spriteDirty = false;
    }

    sdl.setColor(m_innerColor);
    m_inner = m_pos;
    m_inner.w -= 2.f * c_node_radius + 2.f * c_outer_margin;
    m_inner.h -= 2.f * c_outer_margin;
    sdl.drawRectFilled(center(m_inner));

    const float node_input_height = c_node_radius * 2.f * m_maxInput + c_node_spacing * (m_maxInput - 1);
    const float node_left = -m_pos.w / 2.f + c_node_radius + c_outer_margin;
    for (int i = 0; i < m_maxInput; i++) {
        m_inputNodes[i]->p = {m_pos.x + node_left,
                              m_pos.y - node_input_height / 2.f + c_node_radius + i * (c_node_radius * 2.f + c_node_spacing)};
        m_inputNodes[i]->render();
    }
    const float node_output_height = c_node_radius * 2.f * m_maxOutput + c_node_spacing * (m_maxOutput - 1);
    const float node_right = m_pos.w / 2.f - c_node_radius - c_outer_margin;
    for (int i = 0; i < m_maxOutput; i++) {
        m_outputNodes[i]->p = {m_pos.x + node_right,
                               m_pos.y - node_output_height / 2.f + c_node_radius + i * (c_node_radius * 2.f + c_node_spacing)};
        m_outputNodes[i]->render();
    }

    m_text->render({m_pos.x, m_pos.y});
}

void Gate::renderNodeLines() {
    for (auto i : m_inputNodes) {
        i->renderNodeLines();
    }
}

void Gate::createSprite() {
    const float node_input_height = c_node_radius * 2.f * m_maxInput + c_node_spacing * (m_maxInput - 1);
    const float node_output_height = c_node_radius * 2.f * m_maxOutput + c_node_spacing * (m_maxOutput - 1);
    m_pos.h = std::max({node_output_height, node_input_height, TTF_GetFontSize(g_font) + 3});

    std::string uppername;
    for (auto &c : m_name) {
        uppername += toupper(c);
    }

    TTF_GetFontSize(g_font);
    m_text = std::make_unique<RawText>(uppername, SDL_Color(255, 255, 255, 255), g_font, m_pos.h);
    m_pos.h += c_outer_margin;
    m_pos.w = m_text->tex().w() + c_text_margin * 2.f + c_outer_margin;
}

std::vector<bool> Gate::states() const {
    std::vector<bool> states(m_maxOutput);
    for (int i = 0; i < m_maxOutput; i++) {
        states[i] = state(i);
    }
    return states;
}

void InputGate::electrify() { m_innerColor = {Uint8(m_outputNodes[0]->state ? 0 : 255), Uint8(m_outputNodes[0]->state ? 255 : 0), 0, 255}; }

void OutputGate::electrify() {
    m_innerColor = {Uint8(m_outputNodes[0]->state ? 0 : 255), Uint8(m_outputNodes[0]->state ? 255 : 0), 0, 255};
    m_outputNodes[0]->state = m_inputNodes[0]->state();
}

void AndGate::electrify() { m_outputNodes[0]->state = m_inputNodes[0]->state() && m_inputNodes[1]->state(); }

void NotGate::electrify() { m_outputNodes[0]->state = !m_inputNodes[0]->state(); }

CustomGate::CustomGate(std::vector<std::weak_ptr<Gate>> gates, SDL_FPoint pos, std::string name, SDL_Color color) {
    float time = SDL_GetTicks();
    create(gates);
    SDL_Log("time to make custom gate: %f", (SDL_GetTicks() - time));

    m_pos.x = pos.x;
    m_pos.y = pos.y;
    m_name = name;
    m_innerColor = color;
    m_maxInput = m_inputs.size();
    m_maxOutput = m_outputs.size();
}

CustomGate::CustomGate(const CustomGate &other) : Gate(other) {
    std::vector<std::weak_ptr<Gate>> gates;
    gates.insert(gates.end(), other.m_inputs.begin(), other.m_inputs.end());
    gates.insert(gates.end(), other.m_gates.begin(), other.m_gates.end());
    gates.insert(gates.end(), other.m_outputs.begin(), other.m_outputs.end());
    create(gates);

    m_pos = other.m_pos;
    m_inner = other.m_inner;
    m_innerColor = other.m_innerColor;
    m_name = other.m_name;
    m_maxInput = other.m_maxInput;
    m_maxOutput = other.m_maxOutput;
    m_outputNodes = other.m_outputNodes;
    m_spriteDirty = true;

    // resetConnections();

    m_inputNodes = other.m_inputNodes;
    m_outputNodes = other.m_outputNodes;
}

void CustomGate::electrify() {
    for (int i = 0; i < m_inputs.size(); i++) {
        m_inputs[i]->set(m_inputNodes[i]->state());
    }
    for (int i = 0; i < 100; i++) {
        bool dirty = false;
        for (auto g : m_gates) {
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
    for (int i = 0; i < m_outputs.size(); i++) {
        m_outputs[i]->electrify();
        m_outputNodes[i]->state = m_outputs[i]->state(0);
    }
}

std::vector<std::shared_ptr<Gate>> CustomGate::context() {
    std::vector<std::shared_ptr<Gate>> gates;
    gates.insert(gates.end(), m_inputs.begin(), m_inputs.end());
    gates.insert(gates.end(), m_gates.begin(), m_gates.end());
    gates.insert(gates.end(), m_outputs.begin(), m_outputs.end());
    return gates;
}

void CustomGate::create(std::vector<std::weak_ptr<Gate>> &gates) {
    std::vector<std::shared_ptr<Gate>> allGates;
    float totalX = 0;
    float totalY = 0;
    for (int i = 0; i < gates.size(); i++) {
        auto _g = gates[i];
        auto in = std::dynamic_pointer_cast<InputGate>(_g.lock());
        auto out = std::dynamic_pointer_cast<OutputGate>(_g.lock());
        std::shared_ptr<Gate> g = _g.lock()->copy();
        if (in) {
            m_inputs.push_back(std::dynamic_pointer_cast<InputGate>(g));
        } else if (out) {
            m_outputs.push_back(std::dynamic_pointer_cast<OutputGate>(g));
        } else {
            m_gates.push_back(g);
        }
        allGates.push_back(g);

        totalX += _g.lock()->realpos().x;
        totalY += _g.lock()->realpos().y;
    }
    float avX = totalX / gates.size();
    float avY = totalY / gates.size();

	std::sort(m_inputs.begin(), m_inputs.end(), [](std::shared_ptr<InputGate> a, std::shared_ptr<InputGate> b){ return a->realpos().y < b->realpos().y; });
	std::sort(m_outputs.begin(), m_outputs.end(), [](std::shared_ptr<OutputGate> a, std::shared_ptr<OutputGate> b){ return a->realpos().y < b->realpos().y; });

    for (auto g : allGates) {
        for (int i = 0; i < g->maxOutput(); i++) {
            auto outNode = g->getOutputNode(i).lock();
            if (!outNode) {
                continue;
            }
            outNode->owner = g;
            for (int j = 0; j < gates.size(); j++) {
                if (outNode->toGate.lock() == gates[j].lock()) {
                    outNode->toGate = allGates[j];
                }
            }
        }
        for (int i = 0; i < g->maxInput(); i++) {
            auto inNode = g->getInputNode(i).lock();
            inNode->owner = g;
            for (int j = 0; j < gates.size(); j++) {
                if (!inNode->connected.expired() && inNode->connected.lock()->owner.lock() == gates[j].lock()) {
                    for (int k = 0; k < gates[i].lock()->maxOutput(); k++) {
                        if (inNode->connected.lock() == gates[j].lock()->getOutputNode(k).lock()) {
                            inNode->connected = allGates[j]->getOutputNode(k);
                        }
                    }
                }
            }
        }
        g->pos(g->realpos().x - avX / 2.f + 1920.f / 2.f, g->realpos().y - avY / 2.f + 1080.f / 2.f);
    }
}
