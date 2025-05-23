#include "engine/sdl_wrapper.hpp"
#include "gate.hpp"
#include "gate_adder.hpp"
#include "gate_packager.hpp"
#include "globals.hpp"
#include "selection_box.hpp"
#include "sim_context.hpp"
#include <algorithm>
#include <bitset>
#include <format>
// #include "saver.hpp" HOLD

SDL sdl;
TTF_Font *g_font;
SDL_FPoint g_prevMousePos;
SDL_FPoint g_mousePos;

float g_dmouseX;
float g_dmouseY;

std::string g_inputText = "";
bool g_uiActive = false;

SimContext g_context;
SelectionBox g_selectionBox;
GatePackager g_gatePackager;
GateAdder g_gateAdder;

int main() {
    // Saver s;

    g_font = TTF_OpenFont("art/fonts/sourcecodevf/sourcecodevf.ttf", 24.);

    g_context.makeGate("I", {100, 500});
    g_context.makeGate("not", {200, 600});
    g_context.makeGate("O", {200, 400});
    g_context.makeGate("and", {500, 500});

    SDL_FPoint pressMousePosition;
	// left got held during a middle holding
	bool lghdamh = false;

    int iterationCount = 0;

    while (true) {
        Uint64 frameStart = SDL_GetTicks();
        sdl.clear();

		g_context.applyScale();

        SDL_Event e;
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
                            }
                        } break;
                        case SDLK_SPACE: {
                            // if (sdl.textInputActive() && g_gateMap.contains(g_inputText)) {
                            if (sdl.textInputActive()) {
                                sdl.stopTextInput();
                                // g_gateMap[g_inputText]();
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
                g_selectionBox.update(e);
                g_gateAdder.update(e);
                if (!g_selectionBox.selecting()) {
                    // g_nodeLineDrawer.update(e);
                }
            }
            switch (e.type) {
                case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                    auto mouseFlags = SDL_GetMouseState(NULL, NULL);
                    if (mouseFlags & SDL_BUTTON_LEFT) {
                        if (auto mouseInputGate = std::dynamic_pointer_cast<InputGate>(g_context.getGateAt(g_mousePos).lock())) {
                            mouseInputGate->flip();
                        }
                    }
                } break;
                case SDL_EVENT_MOUSE_BUTTON_UP: {
                } break;
                case SDL_EVENT_KEY_DOWN: {
                    switch (e.key.key) {
                        case SDLK_BACKSPACE: {
                            if (g_inputText.size() > 0) {
                                g_inputText.pop_back();
                            }
                        } break;
                        case SDLK_A: { // ADD GATE
                            g_uiActive = true;
                            sdl.startTextInput();
                        } break;
                        case SDLK_B: { // BACK (home pos)
                            g_context.resetPosition();
							g_context.resetScale();
                        } break;
                        case SDLK_RETURN: {
                            g_context.endContext();
                        } break;
                    }
                } break;
                case SDL_EVENT_MOUSE_WHEEL: {
					g_context.scroll(e.wheel.y);
                } break;
            }
        }

        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_MIDDLE) {
			if (!(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LEFT)) {
				if (!lghdamh) {
					g_context.move(g_dmouseX, g_dmouseY);
				}
			} else {
				lghdamh = true;
				int dir = g_dmouseX > 0 ? 1 : (g_dmouseX == 0 ? 0 : -1);
				g_context.scroll(dir);
			}
        } else {
			lghdamh = false;
		}

        g_selectionBox.render();

        int iterations = g_context.electrify();
        iterationCount = iterations ? iterations : iterationCount;

        g_context.renderGateNodeLines();
        g_context.render();

        // UI
        sdl.setRenderScale({1.f, 1.f});
        if (sdl.textInputActive()) {
            if (g_inputText.size() > 0) {
            }
        } else {
            g_inputText = "";
        }

		// DEBUG TEXT
        sdl.setRenderScale({1.5f, 1.5f});
		sdl.setColor({0, 0, 255, 255});
        sdl.renderDebugText({5, 9}, std::format("Iterations: {}", iterationCount));
        sdl.renderDebugText({5, 21}, std::format("Scale: {}", g_context.scale()));
        sdl.renderDebugText({5, 33}, std::format("Gate count: {}", g_context.activeGateCount()));
        sdl.renderDebugText({5, 45}, std::format("Frame time: {}", (SDL_GetTicks() - frameStart)));

        g_gatePackager.render();

        g_prevMousePos = g_mousePos;
        sdl.present();
    }

EXIT:
    TTF_CloseFont(g_font);
    return 0;
}
