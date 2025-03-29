#pragma once

#include "gate.hpp"

struct NodeLine {
    std::weak_ptr<Gate> from;
    int fromIndex;
    bool isInput;
    std::weak_ptr<Gate> to;
    int toIndex;
    std::vector<SDL_FPoint> drawnPositions;
    bool complete;

	void die();
	void moveAll(float dx, float dy);
    void render(SDL_FPoint finalPos);
};

class NodeLineDrawer {
  private:
    std::vector<NodeLine> m_nodeLines;
    bool m_draggingNode;
	SDL_FPoint m_draggingFinalPos;
	SDL_FPoint m_mousePressPos;

    void tintNodeOnMouse();

  public:
    NodeLineDrawer();
    ~NodeLineDrawer();

	bool dragging() const { return m_draggingNode; };

    void update(Uint32);
	void render();
	
	void moveAll(float dx, float dy);

	void deleteNodeLineAtCursor();
    void cancelMovement();
	void straightenLineWithMouseAxis();
};
