#pragma once

#include "gate.hpp"
#include <fstream>

using byte = uint8_t;
class Saver {
  private:
    struct OutputNodeData {
        SDL_FPoint p;
        int connectedGateActiveArrayIndex;
        SDL_Color c;
        std::string name;
        bool state;
    };
    struct InputNodeData {
        SDL_FPoint p;
        int connectedGateActiveArrayIndex;
        int connectedNodeIndex;
        std::vector<SDL_FPoint> nodeLines;
        SDL_Color c;
        std::string name;
    };
    struct GateData {
        int type;
        SDL_FPoint pos;
        SDL_Color color;
        std::string name;
        int typeArrayIndex;

        std::vector<OutputNodeData> outputNodes;
        std::vector<InputNodeData> inputNodes;

        std::vector<GateData> customGateData;
    };

    std::ifstream m_if;
    std::ofstream m_of;

    byte rbyte();
    std::vector<bool> rbools();
    short rshort();
    int rint();
    std::string rstring();
    SDL_FPoint rpoint();
    SDL_Color rcolor();
    GateData rgate();
    std::vector<std::shared_ptr<Gate>> processGates(std::vector<GateData>);

    void wbyte(byte);
    void wbools(std::vector<bool>);
    void wshort(short);
    void wint(int);
    void wstring(std::string);
    void wpoint(SDL_FPoint);
    void wcolor(SDL_Color);
    void wgate(std::shared_ptr<Gate>, const std::vector<std::shared_ptr<Gate>> &context);

  public:
    Saver();
    ~Saver();

    void load();
    void save();
};
