#pragma once

#include "gate.hpp"

class SelectionBox {
  private:
    SDL_FPoint m_start;
    bool m_selecting;

    std::vector<std::weak_ptr<Gate>> m_selection;
	std::vector<std::weak_ptr<Gate>> m_clipboard;

  public:
    bool selecting() const { return m_selecting; }
	std::vector<std::weak_ptr<Gate>> &selected() { return m_selection; }

    void begin();
    void render();
    void end();
	void cancel();

    void moveSelection(float dx, float dy);
    void deleteSelection();
    void copySelection();
    void pasteSelected();
	void straightenSelectionWithSelectionAxis();
	void equalizeDistHor();
	void equalizeDistVer();
	void addToSelection(std::vector<std::weak_ptr<Gate>>);
	void packageSelection();
	void duplicateSelection();

	bool contains(std::weak_ptr<Gate>);
};
