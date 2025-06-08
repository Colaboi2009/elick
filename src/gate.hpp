#pragma once

#include "engine/raw_text.hpp"
#include "helper.hpp"
#include <memory>
#include <vector>

class Gate : public std::enable_shared_from_this<Gate> {
  public:
    struct OutputNode {
        SDL_FPoint p;

        bool state;

        std::weak_ptr<Gate> owner;
        std::weak_ptr<Gate> toGate;

        SDL_Color c{0, 0, 255, 255};
        std::string name{"Output"};

        void render();
		void move(float dx, float dy) { p = {p.x + dx, p.y + dy}; }
		void setPos(float x, float y) { p = {x, y}; }
    };
    struct InputNode {
        SDL_FPoint p;

        std::weak_ptr<Gate> owner;
        std::weak_ptr<OutputNode> connected;

        std::vector<SDL_FPoint> lineNodes;
        SDL_Color c{0, 0, 255, 255};
        std::string name{"Input"};

        bool state() const { return connected.expired() ? false : connected.lock()->state; }
		void move(float dx, float dy);
		void moveAll(float dx, float dy);
		void setPos(float x, float y);

        void render();
		void renderNodeLines();
    };

  protected:
    SDL_FRect m_pos;

    SDL_FRect m_inner;
    SDL_Color m_innerColor;

    std::string m_name;
    std::unique_ptr<RawText> m_text;

    int m_maxInput;
    int m_maxOutput;
    std::vector<std::shared_ptr<InputNode>> m_inputNodes;
    std::vector<std::shared_ptr<OutputNode>> m_outputNodes;

    bool m_spriteDirty{true};
    void createSprite();

    std::shared_ptr<Gate> init();
	std::shared_ptr<Gate> initCopy(const Gate &other);

  public:
    Gate() {}
    Gate(SDL_FPoint pos, int maxIn, int maxOut, std::string name, SDL_Color);
    Gate(const Gate &other);
    ~Gate();
    static std::shared_ptr<Gate> make(SDL_FPoint pos, int maxIn, int maxOut, std::string name, SDL_Color c) {
        return std::make_shared<Gate>(pos, maxIn, maxOut, name, c)->init();
    }

    virtual std::shared_ptr<Gate> copy() const { return std::make_shared<Gate>(*this)->initCopy(*this); }
    virtual std::shared_ptr<Gate> shared() { return shared_from_this(); }

    std::string name() const { return m_name; }
	void name(std::string n) { m_name = n; }
    size_t maxInput() const { return m_maxInput; }
    size_t maxOutput() const { return m_maxOutput; }
	void setInputSize(int i) { m_maxInput = i; }
	void setOutputSize(int i) { m_maxOutput = i; }

    SDL_FRect inner() { return m_inner; }
    SDL_Color color() { return m_innerColor; }
	void color(SDL_Color c) { m_innerColor = c; }

    SDL_FRect renderpos() { return center(m_pos); }
    SDL_FRect realpos() { return m_pos; }
    void move(float dx, float dy);
	void moveAll(float dx, float dy);
    void pos(float x, float y);

    bool state(int i) const { return m_outputNodes[i]->state; }
    std::vector<bool> states() const;

    void connectInput(std::weak_ptr<OutputNode>, std::weak_ptr<InputNode>, std::vector<SDL_FPoint> line);
    void removeInputConnection(int i) { m_inputNodes[i]->connected.reset(); }
    void removeOutputConnection(int i) { m_outputNodes[i]->toGate.reset(); }
    void resetConnections();

	virtual void printDebugInformation();

    std::weak_ptr<InputNode> inputNodeOnPos(SDL_FPoint);
    std::weak_ptr<OutputNode> outputNodeOnPos(SDL_FPoint);
    SDL_FPoint getInputNodePos(int i) const { return m_inputNodes[i]->p; }
    SDL_FPoint getOutputNodePos(int i) const { return m_outputNodes[i]->p; }
    std::weak_ptr<InputNode> getInputNode(int i) const { return m_inputNodes[i]; }
    std::weak_ptr<OutputNode> getOutputNode(int i) const { return m_outputNodes[i]; }
	int getInputNodeIndex(std::weak_ptr<InputNode>);
	int getOutputNodeIndex(std::weak_ptr<OutputNode>);

    virtual void electrify() {};
    void render();
	void renderNodeLines();

  public:
    static constexpr float c_node_radius = 5;
    static constexpr float c_node_spacing = 7;
    static constexpr float c_text_margin = 25;
    static constexpr float c_outer_margin = 5;
    static constexpr SDL_Color c_outerColor = {20, 20, 20, 255};
};

class InputGate : public Gate {
  public:
    InputGate(SDL_FPoint pos) : Gate(pos, 0, 1, "I", {0, 0, 0, 0}) {}
    static std::shared_ptr<InputGate> make(SDL_FPoint pos) {
        return std::dynamic_pointer_cast<InputGate>(std::make_shared<InputGate>(pos)->init());
    }
    InputGate(const InputGate &other) : Gate(other) {}
    std::shared_ptr<Gate> copy() const override { return std::make_shared<InputGate>(*this)->initCopy(*this); }
    std::shared_ptr<Gate> shared() override { return std::make_shared<InputGate>(*this); }
    void set(bool b) { m_outputNodes[0]->state = b; }
    void flip() { m_outputNodes[0]->state = !m_outputNodes[0]->state; }
    void electrify() override;
};

class OutputGate : public Gate {
  public:
    OutputGate(SDL_FPoint pos) : Gate(pos, 1, 1, "O", {0, 0, 0, 0}) {}
    static std::shared_ptr<OutputGate> make(SDL_FPoint pos) {
        return std::dynamic_pointer_cast<OutputGate>(std::make_shared<OutputGate>(pos)->init());
    }
    OutputGate(const OutputGate &other) : Gate(other) {}

    std::shared_ptr<Gate> copy() const override { return std::make_shared<OutputGate>(*this)->initCopy(*this); }
    std::shared_ptr<Gate> shared() override { return std::make_shared<OutputGate>(*this); }
    void electrify() override;
};

class AndGate : public Gate {
  public:
    AndGate(SDL_FPoint pos) : Gate(pos, 2, 1, "AND", {180, 180, 255, 255}) {}
    static std::shared_ptr<AndGate> make(SDL_FPoint pos) {
        return std::dynamic_pointer_cast<AndGate>(std::make_shared<AndGate>(pos)->init());
    }
    AndGate(const AndGate &other) : Gate(other) {}
    std::shared_ptr<Gate> copy() const override { return std::make_shared<AndGate>(*this)->initCopy(*this); }
    std::shared_ptr<Gate> shared() override { return std::make_shared<AndGate>(*this); }
    void electrify() override;
};

class NotGate : public Gate {
  public:
    NotGate(SDL_FPoint pos) : Gate(pos, 1, 1, "NOT", {255, 30, 30, 255}) {}
    static std::shared_ptr<NotGate> make(SDL_FPoint pos) {
        return std::dynamic_pointer_cast<NotGate>(std::make_shared<NotGate>(pos)->init());
    }
    NotGate(const NotGate &other) : Gate(other) {}
    std::shared_ptr<Gate> copy() const override { return std::make_shared<NotGate>(*this)->initCopy(*this); }
    std::shared_ptr<Gate> shared() override { return std::make_shared<NotGate>(*this); }
    void electrify() override;
};

class CustomGate : public Gate {
  private:
    std::vector<std::shared_ptr<Gate>> m_gates;
    std::vector<std::shared_ptr<InputGate>> m_inputs;
    std::vector<std::shared_ptr<OutputGate>> m_outputs;

    void create(const std::vector<std::weak_ptr<Gate>> &);

  public:
    CustomGate(std::vector<std::weak_ptr<Gate>> gates, SDL_FPoint pos, std::string name, SDL_Color color);
    static std::shared_ptr<CustomGate> make(std::vector<std::weak_ptr<Gate>> gates, SDL_FPoint pos, std::string name, SDL_Color color) {
        return std::dynamic_pointer_cast<CustomGate>(std::make_shared<CustomGate>(gates, pos, name, color)->init());
    }
    CustomGate(const CustomGate &other);
    std::shared_ptr<Gate> copy() const override { return std::make_shared<CustomGate>(*this)->initCopy(*this); }
    std::shared_ptr<Gate> shared() override { return std::make_shared<CustomGate>(*this); }

	void updateContext(std::vector<std::weak_ptr<Gate>> gates);

	int rawGateCount() const { return m_gates.size(); }

    void electrify() override;
    std::vector<std::shared_ptr<Gate>> context();

	void printDebugInformation() override;
};
