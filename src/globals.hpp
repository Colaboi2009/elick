#pragma once

#include "engine/sdl_wrapper.hpp"
#include "engine/raw_text.hpp"
#include "gate.hpp"
#include "gate_packager.hpp"

extern SDL sdl;
extern TTF_Font *g_font;
extern std::vector<std::shared_ptr<Gate>> g_gates;
extern SDL_FPoint g_mousePos;
extern SDL_FPoint g_prevMousePos;
extern bool g_uiActive;
extern GatePackager g_gatePackager;

#define TEXT(text, x, y) RawText::Render(text, {x, y}, {255, 255, 255, 255}, g_font)
