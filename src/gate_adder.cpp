#include "gate_adder.hpp"
#include "globals.hpp"
#include <algorithm>

GateAdder::GateAdder() {}

GateAdder::~GateAdder() {}

void GateAdder::begin() {
    g_uiActive = true;
    sdl.startTextInput();
    m_inputText = "";
    m_active = true;

    updatePossibleGateList();
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
        if (PRESSED("backspace")) {
            if (m_inputText.size() > 0) {
                m_inputText.pop_back();
            }
            updatePossibleGateList();
        }
        if (PRESSED("escape")) {
            end();
        }
        if (PRESSED("up")) {
            m_selectedIndex--;
            if (m_selectedIndex < 0) {
                m_selectedIndex = m_possibleGates.size() - 1;
            }
        }
        if (PRESSED("down")) {
            m_selectedIndex++;
            m_selectedIndex %= m_possibleGates.size();
        }
        if (PRESSED("return")) {
            std::string gateName = m_selectedIndex == -1 ? m_inputText : m_possibleGates[m_selectedIndex];
            if (g_context.gateTypeWithNameExists(gateName)) {
                end();
                g_context.makeGate(gateName, g_mousePos);
            }
        }
    } else if (e.type == SDL_EVENT_TEXT_INPUT) {
        m_inputText += e.text.text;
        std::transform(m_inputText.begin(), m_inputText.end(), m_inputText.begin(), toupper);
        updatePossibleGateList();
    }
}

void GateAdder::render() {
    if (!m_active || !sdl.textInputActive()) {
        return;
    }

    const float c_font_height = TTF_GetFontHeight(g_font);

    const float c_width = 400;
    const float c_height = c_font_height;
    const float c_left = 1920 / 2.f - c_width / 2.f;
    const float c_top = 300;

    const float c_left_margin = 5;
    const float c_scale = 1;

    const float c_pred_margin = -2;

    sdl.setColor({30, 30, 30, 255});
    sdl.drawRectFilled({c_left, c_top, c_width, c_height});
    RawText::Render(m_inputText, {c_left + c_left_margin, c_top + c_font_height / 2.f}, true, {255, 255, 255, 255}, g_font);

    const float c_spacing = c_font_height * c_scale;
    const int c_max_shown = 10;
    for (int i = 0; i < m_possibleGates.size(); i++) {
        if (i > c_max_shown) {
            break;
        }

        const float top = c_top + c_spacing * (i + 1);
        if (i == m_selectedIndex) {
            sdl.setColor({40, 40, 40, 255});
        } else {
            sdl.setColor({15, 15, 15, 255});
        }
        sdl.drawRectFilled({c_left, top, c_width, c_height});
        RawText::Render(m_possibleGates[i], {c_left + c_left_margin, top + c_font_height / 2.f}, true, {255, 255, 255, 255}, g_font);
    }
}

void GateAdder::updatePossibleGateList() {
    m_selectedIndex = -1;
    m_possibleGates.clear();

    if (m_inputText == "") {
        // no gates; show all for fun
        m_possibleGates.resize(g_context.gateTypeCount());
        for (int i = 0; i < g_context.gateTypeCount(); i++) {
            m_possibleGates[i] = g_context.getGateType(i)->name();
        }
        return;
    }

    for (int i = 0; i < g_context.gateTypeCount(); i++) {
        std::string name = g_context.getGateType(i)->name();
        if (name.contains(m_inputText)) {
            m_possibleGates.push_back(name);
        }
    }
}
