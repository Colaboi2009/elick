#include "gate_adder.hpp"
#include "globals.hpp"
#include <algorithm>

GateAdder::GateAdder() {
}

GateAdder::~GateAdder() {
}

void GateAdder::begin() {
	g_uiActive = true;
	sdl.startTextInput();
	m_inputText = "";
	m_active = true;
}

void GateAdder::end() {
	g_uiActive = false;
	sdl.stopTextInput();
	m_active = false;
}

void GateAdder::update(SDL_Event e) {
	if (!m_active) {
		return;
	}

	if (e.type == SDL_EVENT_KEY_DOWN) {
		switch (e.key.key) {
			case SDLK_BACKSPACE: {
				if (m_inputText.size() > 0) {
					m_inputText.pop_back();
				}
			} break;
			case SDLK_ESCAPE: {
				end();
			} break;
			case SDLK_RETURN: {
				if (g_context.gateTypeWithNameExists(m_inputText)) {
					end();
					g_context.makeGate(m_inputText, g_mousePos);
				}
			} break;
		}
	} else if (e.type == SDL_EVENT_TEXT_INPUT) {
		m_inputText += e.text.text;
		std::transform(m_inputText.begin(), m_inputText.end(), m_inputText.begin(), toupper);
	}
}

void GateAdder::render() {
	if (!m_active || !sdl.textInputActive()) {
		return;
	}

	const float scale = 2;
	if (m_inputText.size() > 0) {
		Texture t = RawText::Render(m_inputText, {1920 / 2.f, 1080 / 2.f}, false, {255, 255, 255, 255}, g_font);
		sdl.setColor({30, 30, 30, 255});
		sdl.drawRectFilled(center({1920 / 2.f, 1080 / 2.f, t.w() * scale + 10.f, t.h() * scale}));
		t.render({1920 / 2.f, 1080 / 2.f}, scale);
	} else {
		sdl.setColor({30, 30, 30, 255});
		sdl.drawRectFilled(center({1920 / 2.f, 1080 / 2.f, 10, TTF_GetFontSize(g_font) * scale}));
	}
}
