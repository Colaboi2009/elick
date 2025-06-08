#pragma once

#include "engine/sdl_wrapper.hpp"
#include "engine/raw_text.hpp"
#include "gate.hpp"
#include "gate_packager.hpp"
#include "selection_box.hpp"
#include "sim_context.hpp"
#include "keybind_parser.hpp"

#include <functional>
#include <unordered_map>

extern SDL sdl;
extern TTF_Font *g_font;
extern SDL_FPoint g_mousePos;
extern SDL_FPoint g_prevMousePos;
extern bool g_uiActive;
extern float g_dmouseX;
extern float g_dmouseY;

extern KeybindParser g_keybinds;
extern SimContext g_context;
extern GatePackager g_gatePackager;
extern SelectionBox g_selectionBox;

#define TEXT(text, x, y) RawText::Render(text, {x, y}, true, {255, 255, 255, 255}, g_font)
#define PRESSED(name) g_keybinds.pressed(e, name)
