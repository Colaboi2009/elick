#pragma once

#include "gate.hpp"
#include <fstream>

using byte = uint8_t;
class Saver {
private:
	std::ifstream m_if;
	std::ofstream m_of;


	byte rbyte();
	std::vector<bool> r8bool();
	short rshort();
	int rint();
	std::string rstring();

	void wbyte(byte);
	void wbools(std::vector<bool>);
	void wshort(short);
	void wint(int);
	void wstring(std::string);
	void wgate(std::shared_ptr<Gate>);

public:
	Saver();
	~Saver();

	void load();
	void save();
};
