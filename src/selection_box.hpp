#pragma once

#include "gate.hpp"

class SelectionBox {
  private:
	// Gate selection dragging
    SDL_FPoint m_start;
    bool m_selecting = false;
	bool m_dragging = false;

    std::vector<std::weak_ptr<Gate>> m_selection;
	std::vector<std::weak_ptr<Gate>> m_clipboard;

	// Node dragging
	bool m_draggingNode = false;
	bool m_alignNodeLineWithAxis = false;
	SDL_FPoint m_finalNodeDraggingPosition;
	std::vector<SDL_FPoint> m_nodeLines;
	std::weak_ptr<Gate::OutputNode> m_selectedOutputNode;
	std::weak_ptr<Gate::InputNode> m_selectedInputNode;

	bool m_nodeDetected = false;

	// funcs

	// returns whether or not a node was detected
	bool handleNodeDraggingDetection(std::weak_ptr<Gate> g);
	void handleGateDraggingDetection(std::weak_ptr<Gate> g);

	void renderDraggedNodeLines();

    void begin();
    void end();

    void copySelection();
    void pasteSelected();
	// deprecated for now
	void straightenSelectionWithSelectionAxis();
	void equalizeDistHor();
	void equalizeDistVer();
	void packageSelection();
	void editGate();

	void cancelNodeDragging();
	void alignNodeLineWithAxis();

  public:
	SelectionBox();
	~SelectionBox();

	void duplicateSelection();

    bool selecting() const { return m_selecting; }
	std::vector<std::weak_ptr<Gate>> &selected() { return m_selection; }
	bool contains(std::weak_ptr<Gate>);
	bool nodeDetectedAtMouse() const { return m_nodeDetected; }

	void addToSelection(std::vector<std::weak_ptr<Gate>>);

	void update(SDL_Event);
    void render();

	void cancel();
    void moveSelection(float dx, float dy);
    void deleteSelection();

	void updateHomeOffset(float dx, float dy);
};
