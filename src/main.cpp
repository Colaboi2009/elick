#include "engine/sdl_wrapper.hpp"
#include "gate.hpp"
#include "gate_adder.hpp"
#include "gate_packager.hpp"
#include "globals.hpp"
#include "selection_box.hpp"
#include "sim_context.hpp"
 #include "saver.hpp"
#include <algorithm>
#include <bitset>
#include <format>

SDL sdl;
TTF_Font *g_font;
SDL_FPoint g_prevMousePos;
SDL_FPoint g_mousePos;

float g_dmouseX;
float g_dmouseY;

bool g_uiActive = false;

SimContext g_context;
SelectionBox g_selectionBox;
GatePackager g_gatePackager;
GateAdder g_gateAdder;

int main() {
    g_font = TTF_OpenFont("art/fonts/sourcecodevf/sourcecodevf.ttf", 24.);

	Saver saver;
	saver.load();


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
            } else if (g_uiActive) {
                g_gatePackager.update(e);
                g_gateAdder.update(e);
                if (!sdl.textInputActive() && e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE) {
                    g_uiActive = false;
                }
            } else {
                g_selectionBox.update(e);
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
							case ck_add_gate: {
								g_gateAdder.begin();
							} break;
							case ck_home_pos: { // BACK (home pos)
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
		if (g_uiActive) {
			g_gateAdder.render();
			g_gatePackager.render();
		}

		// DEBUG TEXT
        sdl.setRenderScale({1.5f, 1.5f});
		sdl.setColor({0, 0, 255, 255});
        sdl.renderDebugText({5, 9}, std::format("Iterations: {}", iterationCount));
        sdl.renderDebugText({5, 21}, std::format("Scale: {}", g_context.scale()));
        sdl.renderDebugText({5, 33}, std::format("Gate count: {}", g_context.activeGateCount()));
        sdl.renderDebugText({5, 45}, std::format("Frame time: {}", (SDL_GetTicks() - frameStart)));

        g_prevMousePos = g_mousePos;
        sdl.present();
    }

EXIT:
	saver.save();

    TTF_CloseFont(g_font);
    return 0;
}
