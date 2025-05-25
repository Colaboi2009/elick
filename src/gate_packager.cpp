#include "gate_packager.hpp"

#include "globals.hpp"
#include <algorithm>

GatePackager::GatePackager() {}

GatePackager::~GatePackager() {}

void GatePackager::begin(std::vector<std::weak_ptr<Gate>> gates) {
    g_uiActive = true;
    m_active = true;
    m_toPackage = gates;

    m_name = "NAME";
    m_color = {Uint8(rand() % 255), Uint8(rand() % 255), Uint8(rand() % 255)};
}

void GatePackager::end() {
    std::shared_ptr<CustomGate> custom = CustomGate::make(m_toPackage, g_mousePos, m_name, m_color);
    g_context.addNewGate(custom);
    g_context.makeGate(custom->name(), g_mousePos);

    g_selectionBox.deleteSelection();

    m_active = false;
    g_uiActive = false;
}

void GatePackager::update(SDL_Event e) {
    if (!m_active) {
        return;
    }

    if (e.type == SDL_EVENT_KEY_DOWN) {
        if (!sdl.textInputActive()) {
            switch (e.key.key) {
                case SDLK_N: {
                    sdl.startTextInput();
                    m_textPurpose = Purpose::NAME;
                    m_name = "";
                } break;
                case SDLK_R: {
                    sdl.startTextInput();
                    m_textPurpose = Purpose::RED;
                    m_color.r = 0;
                } break;
                case SDLK_G: {
                    sdl.startTextInput();
                    m_textPurpose = Purpose::GREEN;
                    m_color.g = 0;
                } break;
                case SDLK_B: {
                    sdl.startTextInput();
                    m_textPurpose = Purpose::BLUE;
                    m_color.b = 0;
                } break;
                case SDLK_C: {
                    // clear
                } break;
                case SDLK_ESCAPE: {
                    // dont do change
                } break;
                case SDLK_Y: {
					if (!g_context.gateTypeWithNameExists(m_name)) {
						end();
					} else {
						// TODO(ala): warn user about name already existing
					}
                } break;
            }
        } else {
			switch (e.key.key) {
                case SDLK_BACKSPACE: {
                    if (m_inputText.size() > 0) {
                        m_inputText.pop_back();
                        updateTextInputs();
                    }
                } break;
                case SDLK_RETURN: {
                    // do change
                    sdl.stopTextInput();
                    m_inputText = "";
                } break;
			}
		}
    } else if (e.type == SDL_EVENT_TEXT_INPUT) {
        m_inputText += e.text.text;
        updateTextInputs();
    }
}

void GatePackager::render() {
    if (!m_active) {
        return;
    }

    const float c_font_height = TTF_GetFontHeight(g_font) * 2.f;

    const float c_width = 400;

    const float c_name_width = c_width;
    const float c_name_height = 75;

    const float c_color_width = 250;
    const float c_color_height = 50;

    const float c_height = c_name_height + c_color_height * 3;
    const float c_left = (1920 / 2.f) - c_width / 2.f;
    const float c_top = (1080 / 2.f) - c_height / 2.f;

    const float c_color_top = c_top + c_name_height;

    sdl.setColor({30, 30, 30, 255});
    sdl.drawRectFilled({c_left, c_top, c_width, c_height});

    sdl.setColor({50, 50, 50, 255});
    sdl.drawRectFilled({c_left, c_top, c_name_width, c_name_height});
    RawText::Render(m_name, {c_left, c_top + c_font_height / 2.f}, true, {255, 255, 255, 255}, g_font);

    // colors
    sdl.setColor({m_color.r, 0, 0, 255});
    sdl.drawRectFilled({c_left, c_color_top + c_color_height * 0, c_color_width, c_color_height});
    RawText::Render(std::to_string(m_color.r), {c_left, (c_color_top + c_color_height * 0) + c_font_height / 2.f}, true,
                    {255, 255, 255, 255}, g_font);

    sdl.setColor({0, m_color.g, 0, 255});
    sdl.drawRectFilled({c_left, c_color_top + c_color_height * 1, c_color_width, c_color_height});
    RawText::Render(std::to_string(m_color.g), {c_left, (c_color_top + c_color_height * 1) + c_font_height / 2.f}, true,
                    {255, 255, 255, 255}, g_font);

    sdl.setColor({0, 0, m_color.b, 255});
    sdl.drawRectFilled({c_left, c_color_top + c_color_height * 2, c_color_width, c_color_height});
    RawText::Render(std::to_string(m_color.b), {c_left, (c_color_top + c_color_height * 2) + c_font_height / 2.f}, true,
                    {255, 255, 255, 255}, g_font);

    sdl.setColor(m_color);
    sdl.drawRectFilled({c_left + c_color_width, c_top + c_name_height, c_width - c_color_width, c_height - c_name_height});
}

void GatePackager::updateTextInputs() {
    switch (m_textPurpose) {
        case Purpose::NAME: {
            m_name = m_inputText;
            std::transform(m_name.begin(), m_name.end(), m_name.begin(), toupper);
        } break;
            // clang-format off
		case Purpose::RED: try { m_color.r = std::stoi(m_inputText); } catch (...) {} break;
		case Purpose::GREEN: try { m_color.g = std::stoi(m_inputText); } catch (...) {} break;
		case Purpose::BLUE: try { m_color.b = std::stoi(m_inputText); } catch (...) {} break;
            // clang-format on
    }
}
