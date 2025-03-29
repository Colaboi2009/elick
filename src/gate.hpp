#pragma once

#include "engine/raw_text.hpp"
#include "helper.hpp"
#include <memory>
#include <vector>

struct Node {
    static constexpr float c_radius = 10;
    SDL_Color c{0, 0, 255, 255};
    SDL_FPoint p;
    std::string name{""};

    void render();
};

class Gate {
  protected:
    SDL_FRect m_pos;

    SDL_FRect m_inner;
    SDL_Color m_innerColor;

    std::string m_name;
    std::unique_ptr<RawText> m_text;

    int m_maxInput;
    int m_maxOutput;
    std::vector<Node> m_inputNodePos;
    std::vector<Node> m_outputNodePos;
    std::vector<std::weak_ptr<Gate>> m_in;
    std::vector<int> m_inIndices;

    std::vector<bool> m_state{};

    bool m_spriteDirty{true};
    void createSprite();

  public:
	Gate() {}
    Gate(SDL_FRect pos, int maxIn, int maxOut, std::string name, SDL_Color);
    Gate(const Gate &other);
    ~Gate();

	virtual std::shared_ptr<Gate> copy() const { return std::make_shared<Gate>(*this); }

    bool state(int i) const { return m_state[i]; }
    const std::vector<bool> &states() const { return m_state; }
    std::string name() const { return m_name; }
    std::vector<Node> &inputNodes() { return m_inputNodePos; }
    std::vector<Node> &outputNodes() { return m_outputNodePos; }
    std::vector<std::weak_ptr<Gate>> &inGates() { return m_in; }
    SDL_FPoint getNodePos(int index, bool input) { return input ? m_inputNodePos[index].p : m_outputNodePos[index].p; }

    SDL_FRect pos() { return center(m_pos); }
    SDL_FRect realpos() { return m_pos; }
    SDL_FRect inner() { return m_inner; }

    void move(float dx, float dy) { m_pos = {m_pos.x + dx, m_pos.y + dy, m_pos.w, m_pos.h}; }
    void pos(float x, float y) { m_pos = {x, y, m_pos.w, m_pos.h}; }

    void connectInput(std::weak_ptr<Gate> g, int i, int node) {
        m_in[i] = g;
        m_inIndices[i] = node;
    }
	void resetConnections() {
		m_in.clear();
		m_inIndices.clear();
		m_in.resize(m_maxInput);
		m_inIndices.resize(m_maxInput);
	}
    void removeConnection(int i) { m_in[i].reset(); }

    bool onNode(SDL_FPoint, bool *isInput, int *index, Node **);

    virtual void electrify() {};
    void render();

  public:
    static constexpr float c_node_spacing = 7;
    static constexpr float c_text_margin = 25;
    static constexpr float c_outer_margin = 5;
    static constexpr SDL_Color c_outerColor = {20, 20, 20, 255};
};

static bool getState(std::weak_ptr<Gate> g, int i) {
    std::shared_ptr<Gate> gS = g.lock();
    bool is = gS == nullptr ? false : gS->state(i);
    return is;
}

class InputGate : public Gate {
  public:
    InputGate(SDL_FRect pos) : Gate(pos, 0, 1, "Input", {0, 0, 0, 0}) {}
    InputGate(const InputGate &other) : Gate(other) {}
	std::shared_ptr<Gate> copy() const override { return std::make_shared<InputGate>(*this); }
    void set(bool b) { m_state[0] = b; }
    void flip() { m_state[0] = !m_state[0]; }
    void electrify() override { m_innerColor = {Uint8(m_state[0] ? 0 : 255), Uint8(m_state[0] ? 255 : 0), 0, 255}; }
};

class OutputGate : public Gate {
  public:
    OutputGate(SDL_FRect pos) : Gate(pos, 1, 1, "Output", {0, 0, 0, 0}) {}
    OutputGate(const OutputGate &other) : Gate(other) {}
	std::shared_ptr<Gate> copy() const override { return std::make_shared<OutputGate>(*this); }
    void electrify() override {
        m_innerColor = {Uint8(m_state[0] ? 0 : 255), Uint8(m_state[0] ? 255 : 0), 0, 255};
        m_state[0] = getState(m_in[0], m_inIndices[0]);
    }
};

class AndGate : public Gate {
  public:
    AndGate(SDL_FRect pos) : Gate(pos, 2, 1, "And", {180, 180, 255, 255}) {}
    AndGate(const AndGate &other) : Gate(other) {}
	std::shared_ptr<Gate> copy() const override { return std::make_shared<AndGate>(*this); }
    void electrify() override { m_state[0] = getState(m_in[0], m_inIndices[0]) && getState(m_in[1], m_inIndices[0]); }
};

class NotGate : public Gate {
  public:
    NotGate(SDL_FRect pos) : Gate(pos, 1, 1, "Not", {255, 30, 30, 255}) {}
    NotGate(const NotGate &other) : Gate(other) {}
	std::shared_ptr<Gate> copy() const override { return std::make_shared<NotGate>(*this); }
    void electrify() override { m_state[0] = !getState(m_in[0], m_inIndices[0]); }
};

class CustomGate : public Gate {
  private:
    std::vector<std::shared_ptr<Gate>> m_gates;
    std::vector<std::shared_ptr<InputGate>> m_inputs;
    std::vector<std::shared_ptr<OutputGate>> m_outputs;

	void create(std::vector<std::weak_ptr<Gate>>&);
  public:
    CustomGate(std::vector<std::weak_ptr<Gate>> gates, SDL_FRect pos, std::string name, SDL_Color color);
    CustomGate(const CustomGate &other);
	std::shared_ptr<Gate> copy() const override { return std::make_shared<CustomGate>(*this); }

    void electrify() override;
};
