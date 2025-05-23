#include "saver.hpp"
#include <cassert>

#include <cmath>
#include <iostream>

Saver::Saver() {
	load();
}

Saver::~Saver() {
	save();
}

byte Saver::rbyte() {
	char c;
	m_if.read((char *)&c, 1);
	return c;
}
std::vector<bool> Saver::r8bool() {
	short count = rbyte();
	std::vector<byte> bytes(std::ceil(count / 8));
	for (int i = 0; i < count; i++) {
		bytes[i] = rbyte();
	}
	std::vector<bool> bools(count);
	for (int i = 0, j = 0; i < bools.size(); i++) {
		j = i % 8 ? j++ : j; 
		bools[i] = (bytes[j] >> i % 8) & 1;
	}
	return bools;
}
short Saver::rshort() {
	return rbyte() << 8 | rbyte() << 0;
}
int Saver::rint() {
	return rbyte() << 24 | rbyte() << 16 | rbyte() << 8 | rbyte() << 0;
}
std::string Saver::rstring() {
	short size = rshort();
	std::string str;
	for (int i = 0; i < size; i++) {
		str += rbyte();
	}
	return str;
}

void Saver::load() {
	m_if.open("data.piss");
	byte bt = rbyte();
	std::vector<bool> boolst = r8bool();
	short st = rshort();
	int intt = rint();
	std::string strt = rstring();
	bool bools[8];
	for (int i = 0; i < boolst.size(); i++) {
		bools[i] = boolst[i];
	}
	m_if.close();
}

void Saver::wbyte(byte b) {
	m_of.write((char *)&b, 1);
}
void Saver::wbools(std::vector<bool> bools) {
	wbyte((byte)bools.size());
	for (int i = 0; bools.size() - i > 8; i+=8) {
		if (bools.size() - i < 8) {
			int required = 8 - bools.size();
			for (int i = 0; i < required; i++) {
				bools.push_back(0);
			}
		}
		char c{};
		for (int i = 0; i < bools.size(); i++) {
			c |= bools[i] << i;
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
void Saver::wgate(std::shared_ptr<Gate> g) {
	short gateType = -1;
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
	wshort(gateType);
	wint(g->realpos().x);
	wint(g->realpos().y);
	wint(g->color().r);
	wint(g->color().g);
	wint(g->color().b);
	wint(g->color().a);
	wstring(g->name());
	wbools(g->states());
}

void Saver::save() {
	m_of.open("data.piss");
	wbyte(0x24);
	std::vector<bool> bools = {1, 1, 1, 0, 0, 0, 1, 1};
	wbools(bools);
	wshort(0x2442);
	wint(0x24422442);
	wstring("this is funny little bitch");
	m_of.close();
}
