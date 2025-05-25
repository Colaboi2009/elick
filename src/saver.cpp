#include "saver.hpp"
#include "globals.hpp"
#include <cassert>

#include <cmath>
#include <iostream>

Saver::Saver() {
    //save();
    //load();
}

Saver::~Saver() {
    // save();
}

byte Saver::rbyte() {
    char c;
    m_if.read((char *)&c, 1);
    return c;
}

std::vector<bool> Saver::rbools() {
    byte count = rbyte();
    std::vector<byte> bytes(std::ceil((float)count / 8));
    for (int i = 0; i < bytes.size(); i++) {
        bytes[i] = rbyte();
    }
    std::vector<bool> bools(count);
	for (int i = 0; i < bytes.size(); i++) {
		int bitCount = (bools.size() - i * 8 > 8) ? 8 : bools.size() - i * 8;
		for (int j = 0; j < bitCount; j++) {
			bools[i * 8 + j] = (bytes[i] >> j) & 1;
		}
	}
    return bools;
}

short Saver::rshort() { return rbyte() << 8 | rbyte() << 0; }

int Saver::rint() { return rbyte() << 24 | rbyte() << 16 | rbyte() << 8 | rbyte() << 0; }

std::string Saver::rstring() {
    short size = rshort();
    std::string str;
    for (int i = 0; i < size; i++) {
        str += rbyte();
    }
    return str;
}

SDL_FPoint Saver::rpoint() { return {(float)rint(), (float)rint()}; }

SDL_Color Saver::rcolor() { return {rbyte(), rbyte(), rbyte(), rbyte()}; }

Saver::GateData Saver::rgate() {
	GateData gate;

    gate.type = rbyte();
    gate.pos = rpoint();
    gate.color = rcolor();
    gate.name = rstring();
    gate.typeArrayIndex = rint();

    gate.outputNodes.resize(rint());
	for (auto &node : gate.outputNodes) {
		node.p = rpoint();
        node.connectedGateActiveArrayIndex = rint();
		node.c = rcolor();
        node.name = rstring();
    }
    std::vector<bool> states = rbools();
	assert(states.size() == gate.outputNodes.size() && "state count must be equal to output node count!");
	for (int i = 0; i < gate.outputNodes.size(); i++) {
		gate.outputNodes[i].state = states[i];
	}

	gate.inputNodes.resize(rint());
	for (auto &node : gate.inputNodes) {
        node.p = rpoint();
        node.connectedGateActiveArrayIndex = rint();
        node.connectedNodeIndex = rint();
        node.nodeLines.resize(rbyte());
		for (int i = 0; i < node.nodeLines.size(); i++) {
            node.nodeLines[i] = rpoint();
        }
        node.c = rcolor();
        node.name = rstring();
    }

    if (gate.type == 4) {
		gate.customGateData.resize(rint());
		for (int i = 0; i < gate.customGateData.size(); i++) {
			gate.customGateData[i] = rgate();
		}
    }

	return gate;
}

std::vector<std::shared_ptr<Gate>> Saver::processGates(std::vector<GateData> data) {
	std::vector<std::shared_ptr<Gate>> pgates(data.size());;
	for (int i = 0; i < data.size(); i++) {
		switch (data[i].type) {
			case 0: pgates[i] = AndGate::make(data[i].pos); break;
			case 1: pgates[i] = NotGate::make(data[i].pos); break;
			case 2: pgates[i] = InputGate::make(data[i].pos); break;
			case 3: pgates[i] = OutputGate::make(data[i].pos); break;
			case 4: {
				auto context = processGates(data[i].customGateData);
				std::vector<std::weak_ptr<Gate>> contextWeak(context.size());
				for (int j = 0; j < context.size(); j++) {
					contextWeak[j] = context[j];
				}
				pgates[i] = CustomGate::make(contextWeak, data[i].pos, data[i].name, data[i].color);
				g_context.addNewGate(pgates[i]); // auto checks whether or not the gate already exists
			} break;
		}
	}

	// node population; harmless if nodes aren't connected
	for (int i = 0; i < data.size(); i++) {
		for (int j = 0; j < data[i].outputNodes.size(); j++) {
			const auto &outNode = pgates[i]->getOutputNode(j).lock();
			const auto &outData = data[i].outputNodes[j];
			outNode->p = outData.p;
			if (outData.connectedGateActiveArrayIndex != -1) {
				outNode->toGate = pgates[outData.connectedGateActiveArrayIndex];
			}
			outNode->c = outData.c;
			outNode->name = outData.name;
			outNode->state = outData.state;
		}
		for (int j = 0; j < data[i].inputNodes.size(); j++) {
			const auto &inNode = pgates[i]->getInputNode(j).lock();
			const auto &inData = data[i].inputNodes[j];
			inNode->p = inData.p;
			if (inData.connectedGateActiveArrayIndex != -1) {
				inNode->connected = pgates[inData.connectedGateActiveArrayIndex]->getOutputNode(inData.connectedNodeIndex);
				inNode->lineNodes = inData.nodeLines;
			}
			inNode->c = inData.c;
			inNode->name = inData.name;
		}
	}

	return pgates;
}

void Saver::load() {
    SDL_Log("Saver::load");
    m_if.open("data.piss");
	if (m_if.fail()) {
		return;
	}

	std::vector<GateData> gateTypeData(rint());
	for (int i = 0; i < gateTypeData.size(); i++) {
		gateTypeData[i] = rgate();
	}
	processGates(gateTypeData);

	std::vector<GateData> gateData(rint());
	for (int i = 0; i < gateData.size(); i++) {
		gateData[i] = rgate();
	}
	auto context = processGates(gateData);
	g_context.loadContext(context);

    m_if.close();
    SDL_Log("~Saver::load");
}

// ----------------------------------------------------------------------------------------------------------------------------------------

void Saver::wbyte(byte b) { m_of.write((char *)&b, 1); }

void Saver::wbools(std::vector<bool> bools) {
    wbyte((byte)bools.size());
	int byteCount = std::ceil((float)bools.size() / 8);
	for (int i = 0; i < byteCount; i++) {
		byte c = 0;
		int bitCount = (bools.size() - i * 8 > 8) ? 8 : bools.size() - i * 8;
		for (int j = 0; j < bitCount; j++) {
			c |= bools[i * 8 + j] << j;
		}
		wbyte(c);
	}
}

void Saver::wshort(short s) {
    wbyte(s >> 8);
    wbyte(s >> 0);
}

void Saver::wint(int i) {
    wbyte(i >> 24);
    wbyte(i >> 16);
    wbyte(i >> 8);
    wbyte(i >> 0);
}

void Saver::wstring(std::string s) {
    wshort(s.size());
    for (char &c : s) {
        wbyte(c);
    }
}

void Saver::wpoint(SDL_FPoint p) {
    wint(p.x);
    wint(p.y);
}

void Saver::wcolor(SDL_Color c) {
    wbyte(c.r);
    wbyte(c.g);
    wbyte(c.b);
    wbyte(c.a);
}

void Saver::wgate(std::shared_ptr<Gate> g, const std::vector<std::shared_ptr<Gate>> &context) {
    int gateType = 5;
    if (std::dynamic_pointer_cast<AndGate>(g)) {
        gateType = 0;
    } else if (std::dynamic_pointer_cast<NotGate>(g)) {
        gateType = 1;
    } else if (std::dynamic_pointer_cast<InputGate>(g)) {
        gateType = 2;
    } else if (std::dynamic_pointer_cast<OutputGate>(g)) {
        gateType = 3;
    } else if (std::dynamic_pointer_cast<CustomGate>(g)) {
        gateType = 4;
    }
    assert(gateType != -1);
    wbyte(gateType);
    wpoint({g->realpos().x, g->realpos().y});
    wcolor(g->color());
    wstring(g->name());
    wint(g_context.gateNameToIndex(g->name())); // index into gate TYPES array

    wint(g->maxOutput());
    for (int i = 0; i < g->maxOutput(); i++) {
        auto outNode = g->getOutputNode(i).lock();
        wpoint(outNode->p);

		auto toGate = outNode->toGate.lock();
		int j;
		for (j = 0; j < context.size(); j++) {
			if (context[j] == toGate) {
				wint(j); // index into context
				break;
			}
		}
		if (j == context.size()) {
			wint(-1);
		}
        wcolor(outNode->c);
        wstring(outNode->name);
    }
    wbools(g->states());

    wint(g->maxInput());
    for (int i = 0; i < g->maxInput(); i++) {
        auto inNode = g->getInputNode(i).lock();
        wpoint(inNode->p);
		if (inNode->connected.expired()) {
			wint(-1);
			wint(-1);
		} else {
			auto toGate = inNode->connected.lock()->owner.lock();
			int j;
			for (j = 0; j < context.size(); j++) {
				if (context[j] == toGate) {
					wint(j); // index into context
					break;
				}
			}
			if (j == context.size()) {
				assert(false && "should never reach this!");
			}
			wint(toGate->getOutputNodeIndex(inNode->connected));
		}
        wbyte(inNode->lineNodes.size());
        for (auto p : inNode->lineNodes) {
            wpoint(p);
        }
        wcolor(inNode->c);
        wstring(inNode->name);
    }

    // custom gate
    if (gateType == 4) {
		const auto &customContext = std::dynamic_pointer_cast<CustomGate>(g)->context();
		wint(customContext.size());
        for (auto gc : customContext) {
            // dont get scared this is fine
            wgate(gc, customContext);
        }
    }
}

void Saver::save() {
    SDL_Log("Saver::save");
    m_of.open("data.piss");

	wint(g_context.gateTypeCount());
	for (int i = 0; i < g_context.gateTypeCount(); i++) {
		wgate(g_context.getGateType(i), {});
	}

	wint(g_context.activeGateCount());
	for (int i = 0; i < g_context.activeGateCount(); i++) {
		wgate(g_context.activeGates()[i], g_context.activeGates());
	}

    m_of.close();
    SDL_Log("~Saver::save");
}
