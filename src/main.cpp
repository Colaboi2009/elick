#include "engine/sdl_wrapper.hpp"
#include "gate.hpp"
#include "globals.hpp"
#include "helper.hpp"
#include "node_line_drawer.hpp"
#include "selection_box.hpp"
#include "gate_packager.hpp"
#include "gate_adder.hpp"
#include "gate_manipulator.hpp"
#include <functional>
#include <unordered_map>

SDL sdl;
TTF_Font *g_font;
std::vector<std::shared_ptr<Gate>> g_gates;
SDL_FPoint g_prevMousePos;
SDL_FPoint g_mousePos;

SDL_FPoint g_homeOffset = {0, 0};
float g_scale = 1;
float g_dmouseX;
float g_dmouseY;
std::string g_inputText = "";
bool g_uiActive = false;
bool g_addingGate = false;
std::unordered_map<std::string, std::function<void(void)>> g_gateMap = {
	{"AND", [] { g_gates.push_back(std::make_shared<AndGate>(SDL_FRect{g_mousePos.x, g_mousePos.y})); }},
	{"NOT", [] { g_gates.push_back(std::make_shared<NotGate>(SDL_FRect{g_mousePos.x, g_mousePos.y})); }},
	{"INPUT", [] { g_gates.push_back(std::make_shared<InputGate>(SDL_FRect{g_mousePos.x, g_mousePos.y})); }},
	{"OUTPUT", [] { g_gates.push_back(std::make_shared<OutputGate>(SDL_FRect{g_mousePos.x, g_mousePos.y})); }},
};

NodeLineDrawer g_nodeLineDrawer;
SelectionBox g_selectionBox;
GatePackager g_gatePackager;
GateAdder g_gateAdder;
GateManipulator g_gateManipulator;

void moveWorld(float dx, float dy) {
    g_nodeLineDrawer.moveAll(dx, dy);
    for (auto g : g_gates) {
        g->move(dx, dy);
    }
    g_homeOffset.x += dx;
    g_homeOffset.y += dy;
}

int main() {
    g_font = TTF_OpenFont("art/fonts/sourcecodevf/sourcecodevf.ttf", 24.);

    g_gates.push_back(std::make_shared<InputGate>(SDL_FRect{100, 500, 50, 50}));
    g_gates.push_back(std::make_shared<NotGate>(SDL_FRect{200, 600, 100, 50}));
    g_gates.push_back(std::make_shared<NotGate>(SDL_FRect{200, 400, 100, 50}));
    g_gates.push_back(std::make_shared<AndGate>(SDL_FRect{500, 500, 100, 50}));

    SDL_FPoint pressMousePosition;
    SDL_FPoint prevScrollWheel;
    float scrollPos = 10.f;
    bool middleMouseHeld = false;
    std::shared_ptr<Gate> draggedGate = nullptr;
    int iterationCount = 0;

    while (true) {
        sdl.clear();

        g_scale = scrollPos / 10.f;
        sdl.setRenderScale({g_scale, g_scale});
        SDL_GetMouseState(&g_mousePos.x, &g_mousePos.y);
        g_mousePos.x /= g_scale;
        g_mousePos.y /= g_scale;
        g_dmouseX = g_mousePos.x - g_prevMousePos.x;
        g_dmouseY = g_mousePos.y - g_prevMousePos.y;

        SDL_Event e;
		TEXT("SCALE: " + std::to_string(g_scale), 1810, 600);
        while (SDL_PollEvent(&e)) {
			if (e.type == SDL_EVENT_QUIT) {
				goto EXIT;
			}
			if (g_uiActive) {
				g_gatePackager.update(e);
				if (!sdl.textInputActive() && e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE) {
					g_uiActive = false;
				}
				if (e.type == SDL_EVENT_KEY_DOWN) {
					switch (e.key.key) {
						case SDLK_ESCAPE: {
							if (sdl.textInputActive()) {
								sdl.stopTextInput();
								g_addingGate = false;
							}
						} break;
						case SDLK_SPACE: {
							if (sdl.textInputActive() && g_gateMap.contains(g_inputText)) {
								sdl.stopTextInput();
								g_gateMap[g_inputText]();
								g_addingGate = false;
								g_uiActive = false;
							}
						} break;
					}
				} else if (SDL_EVENT_TEXT_INPUT) {
					g_inputText += e.text.text;
					std::transform(g_inputText.begin(), g_inputText.end(), g_inputText.begin(), toupper);
				}

				continue;
			} else {
			}
            switch (e.type) {
                case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                    auto mouseFlags = SDL_GetMouseState(NULL, NULL);
                    if (mouseFlags == SDL_BUTTON_LEFT) {
                        for (auto g : g_gates) {
                            if (pointInFRect(g_mousePos, g->pos())) {
                                draggedGate = g;
                            }
                        }
                        if (!draggedGate && !g_nodeLineDrawer.dragging()) {
                            g_selectionBox.begin();
                        }
                    } else if (mouseFlags == SDL_BUTTON_MIDDLE) {
                        middleMouseHeld = true;
                    }
                } break;
                case SDL_EVENT_MOUSE_BUTTON_UP: {
                    if (middleMouseHeld) {
                        middleMouseHeld = false;
                    }
                    if (g_selectionBox.selecting()) {
                        g_selectionBox.end();
                    }
                    draggedGate = nullptr;
                } break;
                case SDL_EVENT_KEY_DOWN: {
                    switch (e.key.key) {
                        case SDLK_ESCAPE: {
                            if (g_nodeLineDrawer.dragging()) {
                                g_nodeLineDrawer.cancelMovement();
                            }
                        } break;
						case SDLK_BACKSPACE: {
							if (g_inputText.size() > 0) {
								g_inputText.pop_back();
							}
						} break;
                        case SDLK_A: { // ADD GATE
							g_uiActive = true;
							g_addingGate = true;
							sdl.startTextInput();
                        } break;
                        case SDLK_B: { // BACK (home pos)
                            moveWorld(-g_homeOffset.x, -g_homeOffset.y);
                            scrollPos = 10.f;
                        } break;
                        case SDLK_C: { // COPY
                            g_selectionBox.copySelection();
                        } break;
						case SDLK_E: { // OPTIONS
						} break;
						case SDLK_D: { // DUPLICATE (single)
							if (g_selectionBox.selected().size()) {
								g_selectionBox.duplicateSelection();
							}
						} break;
                        case SDLK_H: {
							g_selectionBox.equalizeDistHor();
                        } break;
                        case SDLK_Q: { // DELETE (quit)
                            g_nodeLineDrawer.deleteNodeLineAtCursor();
                            g_selectionBox.deleteSelection();
                        } break;
                        case SDLK_S: { // STRAIGHTEN
                            g_selectionBox.straightenSelectionWithSelectionAxis();
                        } break;
						case SDLK_T: { // PACKAGE
							g_selectionBox.packageSelection();
						} break;
						case SDLK_V: { // PASTE
							g_selectionBox.pasteSelected();
						} break;
						case SDLK_W: { // ALIGN VERTICALLY
							g_selectionBox.equalizeDistVer();
						} break;
                        case SDLK_X: { // CUT
                        } break;
                        case SDLK_Z: { // UNDO
                        } break;
						case SDLK_1: { // QA {AND}
							g_gateMap["AND"]();
						} break;
						case SDLK_2: { // QA {NOT}
							g_gateMap["NOT"]();
						} break;
						case SDLK_3: { // QA {CUSTOM}
							g_gateMap["INPUT"]();
						} break;
						case SDLK_4: { // QA {CUSTOM}
							g_gateMap["OUTPUT"]();
						} break;
						case SDLK_5: { // QA {CUSTOM}
						} break;
                    }
                } break;
                case SDL_EVENT_MOUSE_WHEEL: {
                    scrollPos += (e.wheel.y - prevScrollWheel.y) * g_scale;
					scrollPos = std::max(scrollPos, 1.f);

					float predictedScale = std::max(scrollPos / 10.f, 0.1f);
					float dx = (g_mousePos.x * g_scale + 1920.f / 2.f) * (1.f / predictedScale - 1.f / g_scale);
					float dy = (g_mousePos.y * g_scale + 1080.f / 2.f) * (1.f / predictedScale - 1.f / g_scale);

					moveWorld(dx / 2.f, dy / 2.f);
					g_scale = predictedScale;
					sdl.setRenderScale({g_scale, g_scale});
                } break;
            }
            if (!g_selectionBox.selecting()) {
                g_nodeLineDrawer.update(e.type);
            }
        }

        if (!g_uiActive && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LSHIFT]) {
            g_nodeLineDrawer.straightenLineWithMouseAxis();
        }
        if (draggedGate) {
            if (g_selectionBox.contains(draggedGate)) {
                g_selectionBox.moveSelection(g_dmouseX, g_dmouseY);
            } else {
                g_selectionBox.cancel();
                draggedGate->move(g_dmouseX, g_dmouseY);
            }
        }
        if (middleMouseHeld) {
            moveWorld(g_dmouseX, g_dmouseY);
        }

        g_selectionBox.render();
        g_nodeLineDrawer.render();

        sdl.setColor({255, 255, 255, 255});
        int iterations;
        for (iterations = 0; iterations < 100; iterations++) {
            bool dirty = false;
            for (auto g : g_gates) {
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
        iterationCount = iterations ? iterations : iterationCount;
        for (auto g : g_gates) {
            g->render();
        }

        // UI
        sdl.setRenderScale({1.0f, 1.0f});
        TEXT("Iterations: " + std::to_string(iterationCount), 1810, 15);
		if (sdl.textInputActive()) {
			if (g_inputText.size() > 0) {
				TEXT(g_inputText, 1920 / 2.f, 1080 / 2.f);
			}
		} else {
			g_inputText = "";
		}

		g_gatePackager.render();

        g_prevMousePos = g_mousePos;
        prevScrollWheel = {e.wheel.x, e.wheel.y};
        sdl.present();

    }

EXIT:
    TTF_CloseFont(g_font);
    return 0;
}
