#include "gate.hpp"

#include "engine/raw_text.hpp"
#include "helper.hpp"

#include "globals.hpp"
#include <algorithm>
#include <cassert>

void Node::render() {
	sdl.setColor(c);
	sdl.drawCircle(c_radius, p.x, p.y);
}

Gate::Gate(SDL_FRect pos, int maxIn, int maxOut, std::string name, SDL_Color c) : m_pos{pos}, m_maxInput(maxIn), m_maxOutput(maxOut), m_name{name}, m_innerColor{c} {
	m_inputNodePos.resize(m_maxInput);
	m_in.resize(m_maxInput);
	m_inIndices.resize(m_maxInput);
	m_outputNodePos.resize(m_maxOutput);
	m_state.resize(m_maxOutput);
}

Gate::Gate(const Gate &other) {
    m_pos = other.m_pos;
    m_inner = other.m_inner;
    m_innerColor = other.m_innerColor;
    m_name = other.m_name;
    m_maxInput = other.m_maxInput;
    m_maxOutput = other.m_maxOutput;
    m_inputNodePos = other.m_inputNodePos;
    m_outputNodePos = other.m_outputNodePos;
    m_in = other.m_in;
    m_inIndices = other.m_inIndices;
    m_state = other.m_state;
    m_spriteDirty = true;

	m_inputNodePos.resize(m_maxInput);
	//m_in.clear();
	m_in.resize(m_maxInput);
	m_inIndices.resize(m_maxInput);
	m_outputNodePos.resize(m_maxOutput);
	m_state.resize(m_maxOutput);
}

Gate::~Gate() {
}

bool Gate::onNode(SDL_FPoint mouseP, bool *isInput, int *index, Node **n) {
	for (int i = 0; i < m_maxInput; i++) {
		if (pointInCircle(mouseP, Node::c_radius, m_inputNodePos[i].p)) {
			if (isInput) {
				*isInput = true;
			}
			if (index) {
				*index = i;
			}
			if (n) {
				*n = &m_inputNodePos[i];
			}
			return true;
		}
	}
	for (int i = 0; i < m_maxOutput; i++) {
		if (pointInCircle(mouseP, Node::c_radius, m_outputNodePos[i].p)) {
			if (isInput) {
				*isInput = false;
			}
			if (index) {
				*index = i;
			}
			if (n) {
				*n = &m_outputNodePos[i];
			}
			return true;
		}
	}
	return false;
}

void Gate::render() {
	if (m_spriteDirty) {
		createSprite();
		m_spriteDirty = false;
	}

	sdl.setColor(m_innerColor);
	m_inner = m_pos;
	m_inner.w -= 2.f * Node::c_radius + 2.f * c_outer_margin;
	m_inner.h -= 2.f * c_outer_margin;
	sdl.drawRectFilled(center(m_inner));

	const float node_input_height = Node::c_radius * 2.f * m_maxInput + c_node_spacing * (m_maxInput - 1);
	const float node_left = -m_pos.w / 2.f + Node::c_radius + c_outer_margin;
	for (int i = 0; i < m_maxInput; i++) {
		m_inputNodePos[i].p = {m_pos.x + node_left, m_pos.y - node_input_height / 2.f + Node::c_radius + i * (Node::c_radius * 2.f + c_node_spacing)};
		m_inputNodePos[i].render();
	}
	const float node_output_height = Node::c_radius * 2.f * m_maxOutput + c_node_spacing * (m_maxOutput - 1);
	const float node_right = m_pos.w / 2.f - Node::c_radius - c_outer_margin;
	for (int i = 0; i < m_maxOutput; i++) {
		m_outputNodePos[i].p = {m_pos.x + node_right, m_pos.y - node_output_height / 2.f + Node::c_radius + i * (Node::c_radius * 2.f + c_node_spacing)};
		m_outputNodePos[i].render();
	}

	m_text->render({m_pos.x, m_pos.y});
}

void Gate::createSprite() {
	const float node_input_height = Node::c_radius * 2.f * m_maxInput + c_node_spacing * (m_maxInput - 1);
	const float node_output_height = Node::c_radius * 2.f * m_maxOutput + c_node_spacing * (m_maxOutput - 1);
	m_pos.h = std::max({node_output_height, node_input_height, TTF_GetFontSize(g_font) + 3});

	TTF_GetFontSize(g_font);
	m_text = std::make_unique<RawText>(m_name, SDL_Color(255, 255, 255, 255), g_font, m_pos.h);
	m_pos.h += c_outer_margin;
	m_pos.w = m_text->tex().w() + c_text_margin * 2.f + c_outer_margin;
}

CustomGate::CustomGate(std::vector<std::weak_ptr<Gate>> gates,SDL_FRect pos, std::string name, SDL_Color color) {
	create(gates);

	m_pos = pos;
	m_maxInput = m_inputs.size();
	m_maxOutput = m_outputs.size();
	m_name = name;
	m_innerColor = color;
	m_inputNodePos.resize(m_maxInput);
	m_in.resize(m_maxInput);
	m_inIndices.resize(m_maxInput);
	m_outputNodePos.resize(m_maxOutput);
	m_state.resize(m_maxOutput);
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
	m_inputNodePos = other.m_inputNodePos;
	m_outputNodePos = other.m_outputNodePos;
	m_in = other.m_in;
	m_inIndices = other.m_inIndices;
	m_state = other.m_state;
	m_spriteDirty = true;

	m_inputNodePos.resize(m_maxInput);
	m_in.resize(m_maxInput);
	m_inIndices.resize(m_maxInput);
	m_outputNodePos.resize(m_maxOutput);
	m_state.resize(m_maxOutput);
}

void CustomGate::electrify() {
	for (int i = 0; i < m_inputs.size(); i++) {
		m_inputs[i]->set(getState(m_in[i], m_inIndices[i]));
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
		m_state[i] = m_outputs[i]->state(0);
	}
}

void CustomGate::create(std::vector<std::weak_ptr<Gate>> &gates) {
	std::vector<std::shared_ptr<Gate>> allGates;
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
	}
	for (auto g : allGates) {
		for (int i = 0; i < g->inGates().size(); i++) {
			for (int j = 0; j < gates.size(); j++) {
				if (g->inGates()[i].lock() == gates[j].lock()) {
					g->inGates()[i] = allGates[j];
					break;
				}
			}
		}
	}
}
