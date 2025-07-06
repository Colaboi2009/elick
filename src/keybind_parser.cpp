#include "keybind_parser.hpp"

#include "globals.hpp"

#include <SDL3/SDL_log.h>
#include <cassert>
#include <fstream>

KeybindParser::KeybindParser() {}

KeybindParser::~KeybindParser() {}

SDL_Keycode KeybindParser::get(std::string name) {
	SDL_Log("keybind %s", name.c_str());
	assert(c_keybind_set.contains(name) && "keybind doesn't exit");
	return m_keybinds[name];
}

bool KeybindParser::pressed(SDL_Event &e, std::string name) {
	return e.key.key == get(name);
}

void KeybindParser::loadKeybinds() {
    std::ifstream file(c_config_filepath);

    while (true) {
        std::string line;
        std::getline(file, line);
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
        if (line.contains("keybinds{")) {
            break;
        }
    }
	int count = 0;
    while (true) {
        std::string line;
        std::getline(file, line);
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

		if (line.contains('}')) {
			break;
		}
		if (line == "") {
			continue;
		}
		count++;

		int equalPos = line.find('=');
		std::string name = line.substr(0, equalPos);
		std::string key = line.substr(equalPos + 1);

		if (!c_keybind_set.contains(name)) {
			sdl.error("Invalid config!", std::format("they keybind {} does not exist!", name));
		}

		// prepare for the worst thing in your entire fucking life (rip line count) I CANT EVEN USE SWITCHES; THE MACRO MADE ME HO-
		// clang-format off
		if (key == "UNKNOWN") m_keybinds[name] = SDLK_UNKNOWN;
		else if (key == "RETURN") m_keybinds[name] = SDLK_RETURN;
		else if (key == "ESCAPE") m_keybinds[name] = SDLK_ESCAPE;
		else if (key == "BACKSPACE") m_keybinds[name] = SDLK_BACKSPACE;
		else if (key == "TAB") m_keybinds[name] = SDLK_TAB;
		else if (key == "SPACE") m_keybinds[name] = SDLK_SPACE;
		else if (key == "EXCLAIM") m_keybinds[name] = SDLK_EXCLAIM;
		else if (key == "DBLAPOSTROPHE") m_keybinds[name] = SDLK_DBLAPOSTROPHE;
		else if (key == "HASH") m_keybinds[name] = SDLK_HASH;
		else if (key == "DOLLAR") m_keybinds[name] = SDLK_DOLLAR;
		else if (key == "PERCENT") m_keybinds[name] = SDLK_PERCENT;
		else if (key == "AMPERSAND") m_keybinds[name] = SDLK_AMPERSAND;
		else if (key == "APOSTROPHE") m_keybinds[name] = SDLK_APOSTROPHE;
		else if (key == "LEFTPAREN") m_keybinds[name] = SDLK_LEFTPAREN;
		else if (key == "RIGHTPAREN") m_keybinds[name] = SDLK_RIGHTPAREN;
		else if (key == "ASTERISK") m_keybinds[name] = SDLK_ASTERISK;
		else if (key == "PLUS") m_keybinds[name] = SDLK_PLUS;
		else if (key == "COMMA") m_keybinds[name] = SDLK_COMMA;
		else if (key == "MINUS") m_keybinds[name] = SDLK_MINUS;
		else if (key == "PERIOD") m_keybinds[name] = SDLK_PERIOD;
		else if (key == "SLASH") m_keybinds[name] = SDLK_SLASH;
		else if (key == "0") m_keybinds[name] = SDLK_0;
		else if (key == "1") m_keybinds[name] = SDLK_1;
		else if (key == "2") m_keybinds[name] = SDLK_2;
		else if (key == "3") m_keybinds[name] = SDLK_3;
		else if (key == "4") m_keybinds[name] = SDLK_4;
		else if (key == "5") m_keybinds[name] = SDLK_5;
		else if (key == "6") m_keybinds[name] = SDLK_6;
		else if (key == "7") m_keybinds[name] = SDLK_7;
		else if (key == "8") m_keybinds[name] = SDLK_8;
		else if (key == "9") m_keybinds[name] = SDLK_9;
		else if (key == "COLON") m_keybinds[name] = SDLK_COLON;
		else if (key == "SEMICOLON") m_keybinds[name] = SDLK_SEMICOLON;
		else if (key == "LESS") m_keybinds[name] = SDLK_LESS;
		else if (key == "EQUALS") m_keybinds[name] = SDLK_EQUALS;
		else if (key == "GREATER") m_keybinds[name] = SDLK_GREATER;
		else if (key == "QUESTION") m_keybinds[name] = SDLK_QUESTION;
		else if (key == "AT") m_keybinds[name] = SDLK_AT;
		else if (key == "LEFTBRACKET") m_keybinds[name] = SDLK_LEFTBRACKET;
		else if (key == "BACKSLASH") m_keybinds[name] = SDLK_BACKSLASH;
		else if (key == "RIGHTBRACKET") m_keybinds[name] = SDLK_RIGHTBRACKET;
		else if (key == "CARET") m_keybinds[name] = SDLK_CARET;
		else if (key == "UNDERSCORE") m_keybinds[name] = SDLK_UNDERSCORE;
		else if (key == "GRAVE") m_keybinds[name] = SDLK_GRAVE;
		else if (key == "A") m_keybinds[name] = SDLK_A;
		else if (key == "B") m_keybinds[name] = SDLK_B;
		else if (key == "C") m_keybinds[name] = SDLK_C;
		else if (key == "D") m_keybinds[name] = SDLK_D;
		else if (key == "E") m_keybinds[name] = SDLK_E;
		else if (key == "F") m_keybinds[name] = SDLK_F;
		else if (key == "G") m_keybinds[name] = SDLK_G;
		else if (key == "H") m_keybinds[name] = SDLK_H;
		else if (key == "I") m_keybinds[name] = SDLK_I;
		else if (key == "J") m_keybinds[name] = SDLK_J;
		else if (key == "K") m_keybinds[name] = SDLK_K;
		else if (key == "L") m_keybinds[name] = SDLK_L;
		else if (key == "M") m_keybinds[name] = SDLK_M;
		else if (key == "N") m_keybinds[name] = SDLK_N;
		else if (key == "O") m_keybinds[name] = SDLK_O;
		else if (key == "P") m_keybinds[name] = SDLK_P;
		else if (key == "Q") m_keybinds[name] = SDLK_Q;
		else if (key == "R") m_keybinds[name] = SDLK_R;
		else if (key == "S") m_keybinds[name] = SDLK_S;
		else if (key == "T") m_keybinds[name] = SDLK_T;
		else if (key == "U") m_keybinds[name] = SDLK_U;
		else if (key == "V") m_keybinds[name] = SDLK_V;
		else if (key == "W") m_keybinds[name] = SDLK_W;
		else if (key == "X") m_keybinds[name] = SDLK_X;
		else if (key == "Y") m_keybinds[name] = SDLK_Y;
		else if (key == "Z") m_keybinds[name] = SDLK_Z;
		else if (key == "LEFTBRACE") m_keybinds[name] = SDLK_LEFTBRACE;
		else if (key == "PIPE") m_keybinds[name] = SDLK_PIPE;
		else if (key == "RIGHTBRACE") m_keybinds[name] = SDLK_RIGHTBRACE;
		else if (key == "TILDE") m_keybinds[name] = SDLK_TILDE;
		else if (key == "DELETE") m_keybinds[name] = SDLK_DELETE;
		else if (key == "PLUSMINUS") m_keybinds[name] = SDLK_PLUSMINUS;
		else if (key == "CAPSLOCK") m_keybinds[name] = SDLK_CAPSLOCK;
		else if (key == "F1") m_keybinds[name] = SDLK_F1;
		else if (key == "F2") m_keybinds[name] = SDLK_F2;
		else if (key == "F3") m_keybinds[name] = SDLK_F3;
		else if (key == "F4") m_keybinds[name] = SDLK_F4;
		else if (key == "F5") m_keybinds[name] = SDLK_F5;
		else if (key == "F6") m_keybinds[name] = SDLK_F6;
		else if (key == "F7") m_keybinds[name] = SDLK_F7;
		else if (key == "F8") m_keybinds[name] = SDLK_F8;
		else if (key == "F9") m_keybinds[name] = SDLK_F9;
		else if (key == "F10") m_keybinds[name] = SDLK_F10;
		else if (key == "F11") m_keybinds[name] = SDLK_F11;
		else if (key == "F12") m_keybinds[name] = SDLK_F12;
		else if (key == "PRINTSCREEN") m_keybinds[name] = SDLK_PRINTSCREEN;
		else if (key == "SCROLLLOCK") m_keybinds[name] = SDLK_SCROLLLOCK;
		else if (key == "PAUSE") m_keybinds[name] = SDLK_PAUSE;
		else if (key == "INSERT") m_keybinds[name] = SDLK_INSERT;
		else if (key == "HOME") m_keybinds[name] = SDLK_HOME;
		else if (key == "PAGEUP") m_keybinds[name] = SDLK_PAGEUP;
		else if (key == "END") m_keybinds[name] = SDLK_END;
		else if (key == "PAGEDOWN") m_keybinds[name] = SDLK_PAGEDOWN;
		else if (key == "RIGHT") m_keybinds[name] = SDLK_RIGHT;
		else if (key == "LEFT") m_keybinds[name] = SDLK_LEFT;
		else if (key == "DOWN") m_keybinds[name] = SDLK_DOWN;
		else if (key == "UP") m_keybinds[name] = SDLK_UP;
		else if (key == "NUMLOCKCLEAR") m_keybinds[name] = SDLK_NUMLOCKCLEAR;
		else if (key == "KP_DIVIDE") m_keybinds[name] = SDLK_KP_DIVIDE;
		else if (key == "KP_MULTIPLY") m_keybinds[name] = SDLK_KP_MULTIPLY;
		else if (key == "KP_MINUS") m_keybinds[name] = SDLK_KP_MINUS;
		else if (key == "KP_PLUS") m_keybinds[name] = SDLK_KP_PLUS;
		else if (key == "KP_ENTER") m_keybinds[name] = SDLK_KP_ENTER;
		else if (key == "KP_1") m_keybinds[name] = SDLK_KP_1;
		else if (key == "KP_2") m_keybinds[name] = SDLK_KP_2;
		else if (key == "KP_3") m_keybinds[name] = SDLK_KP_3;
		else if (key == "KP_4") m_keybinds[name] = SDLK_KP_4;
		else if (key == "KP_5") m_keybinds[name] = SDLK_KP_5;
		else if (key == "KP_6") m_keybinds[name] = SDLK_KP_6;
		else if (key == "KP_7") m_keybinds[name] = SDLK_KP_7;
		else if (key == "KP_8") m_keybinds[name] = SDLK_KP_8;
		else if (key == "KP_9") m_keybinds[name] = SDLK_KP_9;
		else if (key == "KP_0") m_keybinds[name] = SDLK_KP_0;
		else if (key == "KP_PERIOD") m_keybinds[name] = SDLK_KP_PERIOD;
		else if (key == "APPLICATION") m_keybinds[name] = SDLK_APPLICATION;
		else if (key == "POWER") m_keybinds[name] = SDLK_POWER;
		else if (key == "KP_EQUALS") m_keybinds[name] = SDLK_KP_EQUALS;
		else if (key == "F13") m_keybinds[name] = SDLK_F13;
		else if (key == "F14") m_keybinds[name] = SDLK_F14;
		else if (key == "F15") m_keybinds[name] = SDLK_F15;
		else if (key == "F16") m_keybinds[name] = SDLK_F16;
		else if (key == "F17") m_keybinds[name] = SDLK_F17;
		else if (key == "F18") m_keybinds[name] = SDLK_F18;
		else if (key == "F19") m_keybinds[name] = SDLK_F19;
		else if (key == "F20") m_keybinds[name] = SDLK_F20;
		else if (key == "F21") m_keybinds[name] = SDLK_F21;
		else if (key == "F22") m_keybinds[name] = SDLK_F22;
		else if (key == "F23") m_keybinds[name] = SDLK_F23;
		else if (key == "F24") m_keybinds[name] = SDLK_F24;
		else if (key == "EXECUTE") m_keybinds[name] = SDLK_EXECUTE;
		else if (key == "HELP") m_keybinds[name] = SDLK_HELP;
		else if (key == "MENU") m_keybinds[name] = SDLK_MENU;
		else if (key == "SELECT") m_keybinds[name] = SDLK_SELECT;
		else if (key == "STOP") m_keybinds[name] = SDLK_STOP;
		else if (key == "AGAIN") m_keybinds[name] = SDLK_AGAIN;
		else if (key == "UNDO") m_keybinds[name] = SDLK_UNDO;
		else if (key == "CUT") m_keybinds[name] = SDLK_CUT;
		else if (key == "COPY") m_keybinds[name] = SDLK_COPY;
		else if (key == "PASTE") m_keybinds[name] = SDLK_PASTE;
		else if (key == "FIND") m_keybinds[name] = SDLK_FIND;
		else if (key == "MUTE") m_keybinds[name] = SDLK_MUTE;
		else if (key == "VOLUMEUP") m_keybinds[name] = SDLK_VOLUMEUP;
		else if (key == "VOLUMEDOWN") m_keybinds[name] = SDLK_VOLUMEDOWN;
		else if (key == "KP_COMMA") m_keybinds[name] = SDLK_KP_COMMA;
		else if (key == "KP_EQUALSAS400") m_keybinds[name] = SDLK_KP_EQUALSAS400;
		else if (key == "ALTERASE") m_keybinds[name] = SDLK_ALTERASE;
		else if (key == "SYSREQ") m_keybinds[name] = SDLK_SYSREQ;
		else if (key == "CANCEL") m_keybinds[name] = SDLK_CANCEL;
		else if (key == "CLEAR") m_keybinds[name] = SDLK_CLEAR;
		else if (key == "PRIOR") m_keybinds[name] = SDLK_PRIOR;
		else if (key == "RETURN2") m_keybinds[name] = SDLK_RETURN2;
		else if (key == "SEPARATOR") m_keybinds[name] = SDLK_SEPARATOR;
		else if (key == "OUT") m_keybinds[name] = SDLK_OUT;
		else if (key == "OPER") m_keybinds[name] = SDLK_OPER;
		else if (key == "CLEARAGAIN") m_keybinds[name] = SDLK_CLEARAGAIN;
		else if (key == "CRSEL") m_keybinds[name] = SDLK_CRSEL;
		else if (key == "EXSEL") m_keybinds[name] = SDLK_EXSEL;
		else if (key == "KP_00") m_keybinds[name] = SDLK_KP_00;
		else if (key == "KP_000") m_keybinds[name] = SDLK_KP_000;
		else if (key == "THOUSANDSSEPARATOR") m_keybinds[name] = SDLK_THOUSANDSSEPARATOR;
		else if (key == "DECIMALSEPARATOR") m_keybinds[name] = SDLK_DECIMALSEPARATOR;
		else if (key == "CURRENCYUNIT") m_keybinds[name] = SDLK_CURRENCYUNIT;
		else if (key == "CURRENCYSUBUNIT") m_keybinds[name] = SDLK_CURRENCYSUBUNIT;
		else if (key == "KP_LEFTPAREN") m_keybinds[name] = SDLK_KP_LEFTPAREN;
		else if (key == "KP_RIGHTPAREN") m_keybinds[name] = SDLK_KP_RIGHTPAREN;
		else if (key == "KP_LEFTBRACE") m_keybinds[name] = SDLK_KP_LEFTBRACE;
		else if (key == "KP_RIGHTBRACE") m_keybinds[name] = SDLK_KP_RIGHTBRACE;
		else if (key == "KP_TAB") m_keybinds[name] = SDLK_KP_TAB;
		else if (key == "KP_BACKSPACE") m_keybinds[name] = SDLK_KP_BACKSPACE;
		else if (key == "KP_A") m_keybinds[name] = SDLK_KP_A;
		else if (key == "KP_B") m_keybinds[name] = SDLK_KP_B;
		else if (key == "KP_C") m_keybinds[name] = SDLK_KP_C;
		else if (key == "KP_D") m_keybinds[name] = SDLK_KP_D;
		else if (key == "KP_E") m_keybinds[name] = SDLK_KP_E;
		else if (key == "KP_F") m_keybinds[name] = SDLK_KP_F;
		else if (key == "KP_XOR") m_keybinds[name] = SDLK_KP_XOR;
		else if (key == "KP_POWER") m_keybinds[name] = SDLK_KP_POWER;
		else if (key == "KP_PERCENT") m_keybinds[name] = SDLK_KP_PERCENT;
		else if (key == "KP_LESS") m_keybinds[name] = SDLK_KP_LESS;
		else if (key == "KP_GREATER") m_keybinds[name] = SDLK_KP_GREATER;
		else if (key == "KP_AMPERSAND") m_keybinds[name] = SDLK_KP_AMPERSAND;
		else if (key == "KP_DBLAMPERSAND") m_keybinds[name] = SDLK_KP_DBLAMPERSAND;
		else if (key == "KP_VERTICALBAR") m_keybinds[name] = SDLK_KP_VERTICALBAR;
		else if (key == "KP_DBLVERTICALBAR") m_keybinds[name] = SDLK_KP_DBLVERTICALBAR;
		else if (key == "KP_COLON") m_keybinds[name] = SDLK_KP_COLON;
		else if (key == "KP_HASH") m_keybinds[name] = SDLK_KP_HASH;
		else if (key == "KP_SPACE") m_keybinds[name] = SDLK_KP_SPACE;
		else if (key == "KP_AT") m_keybinds[name] = SDLK_KP_AT;
		else if (key == "KP_EXCLAM") m_keybinds[name] = SDLK_KP_EXCLAM;
		else if (key == "KP_MEMSTORE") m_keybinds[name] = SDLK_KP_MEMSTORE;
		else if (key == "KP_MEMRECALL") m_keybinds[name] = SDLK_KP_MEMRECALL;
		else if (key == "KP_MEMCLEAR") m_keybinds[name] = SDLK_KP_MEMCLEAR;
		else if (key == "KP_MEMADD") m_keybinds[name] = SDLK_KP_MEMADD;
		else if (key == "KP_MEMSUBTRACT") m_keybinds[name] = SDLK_KP_MEMSUBTRACT;
		else if (key == "KP_MEMMULTIPLY") m_keybinds[name] = SDLK_KP_MEMMULTIPLY;
		else if (key == "KP_MEMDIVIDE") m_keybinds[name] = SDLK_KP_MEMDIVIDE;
		else if (key == "KP_PLUSMINUS") m_keybinds[name] = SDLK_KP_PLUSMINUS;
		else if (key == "KP_CLEAR") m_keybinds[name] = SDLK_KP_CLEAR;
		else if (key == "KP_CLEARENTRY") m_keybinds[name] = SDLK_KP_CLEARENTRY;
		else if (key == "KP_BINARY") m_keybinds[name] = SDLK_KP_BINARY;
		else if (key == "KP_OCTAL") m_keybinds[name] = SDLK_KP_OCTAL;
		else if (key == "KP_DECIMAL") m_keybinds[name] = SDLK_KP_DECIMAL;
		else if (key == "KP_HEXADECIMAL") m_keybinds[name] = SDLK_KP_HEXADECIMAL;
		else if (key == "LCTRL") m_keybinds[name] = SDLK_LCTRL;
		else if (key == "LSHIFT") m_keybinds[name] = SDLK_LSHIFT;
		else if (key == "LALT") m_keybinds[name] = SDLK_LALT;
		else if (key == "LGUI") m_keybinds[name] = SDLK_LGUI;
		else if (key == "RCTRL") m_keybinds[name] = SDLK_RCTRL;
		else if (key == "RSHIFT") m_keybinds[name] = SDLK_RSHIFT;
		else if (key == "RALT") m_keybinds[name] = SDLK_RALT;
		else if (key == "RGUI") m_keybinds[name] = SDLK_RGUI;
		else if (key == "MODE") m_keybinds[name] = SDLK_MODE;
		else if (key == "SLEEP") m_keybinds[name] = SDLK_SLEEP;
		else if (key == "WAKE") m_keybinds[name] = SDLK_WAKE;
		else if (key == "CHANNEL_INCREMENT") m_keybinds[name] = SDLK_CHANNEL_INCREMENT;
		else if (key == "CHANNEL_DECREMENT") m_keybinds[name] = SDLK_CHANNEL_DECREMENT;
		else if (key == "MEDIA_PLAY") m_keybinds[name] = SDLK_MEDIA_PLAY;
		else if (key == "MEDIA_PAUSE") m_keybinds[name] = SDLK_MEDIA_PAUSE;
		else if (key == "MEDIA_RECORD") m_keybinds[name] = SDLK_MEDIA_RECORD;
		else if (key == "MEDIA_FAST_FORWARD") m_keybinds[name] = SDLK_MEDIA_FAST_FORWARD;
		else if (key == "MEDIA_REWIND") m_keybinds[name] = SDLK_MEDIA_REWIND;
		else if (key == "MEDIA_NEXT_TRACK") m_keybinds[name] = SDLK_MEDIA_NEXT_TRACK;
		else if (key == "MEDIA_PREVIOUS_TRACK") m_keybinds[name] = SDLK_MEDIA_PREVIOUS_TRACK;
		else if (key == "MEDIA_STOP") m_keybinds[name] = SDLK_MEDIA_STOP;
		else if (key == "MEDIA_EJECT") m_keybinds[name] = SDLK_MEDIA_EJECT;
		else if (key == "MEDIA_PLAY_PAUSE") m_keybinds[name] = SDLK_MEDIA_PLAY_PAUSE;
		else if (key == "MEDIA_SELECT") m_keybinds[name] = SDLK_MEDIA_SELECT;
		else if (key == "AC_NEW") m_keybinds[name] = SDLK_AC_NEW;
		else if (key == "AC_OPEN") m_keybinds[name] = SDLK_AC_OPEN;
		else if (key == "AC_CLOSE") m_keybinds[name] = SDLK_AC_CLOSE;
		else if (key == "AC_EXIT") m_keybinds[name] = SDLK_AC_EXIT;
		else if (key == "AC_SAVE") m_keybinds[name] = SDLK_AC_SAVE;
		else if (key == "AC_PRINT") m_keybinds[name] = SDLK_AC_PRINT;
		else if (key == "AC_PROPERTIES") m_keybinds[name] = SDLK_AC_PROPERTIES;
		else if (key == "AC_SEARCH") m_keybinds[name] = SDLK_AC_SEARCH;
		else if (key == "AC_HOME") m_keybinds[name] = SDLK_AC_HOME;
		else if (key == "AC_BACK") m_keybinds[name] = SDLK_AC_BACK;
		else if (key == "AC_FORWARD") m_keybinds[name] = SDLK_AC_FORWARD;
		else if (key == "AC_STOP") m_keybinds[name] = SDLK_AC_STOP;
		else if (key == "AC_REFRESH") m_keybinds[name] = SDLK_AC_REFRESH;
		else if (key == "AC_BOOKMARKS") m_keybinds[name] = SDLK_AC_BOOKMARKS;
		else if (key == "SOFTLEFT") m_keybinds[name] = SDLK_SOFTLEFT;
		else if (key == "SOFTRIGHT") m_keybinds[name] = SDLK_SOFTRIGHT;
		else if (key == "CALL") m_keybinds[name] = SDLK_CALL;
		else if (key == "ENDCALL") m_keybinds[name] = SDLK_ENDCALL;
		else if (key == "LEFT_TAB") m_keybinds[name] = SDLK_LEFT_TAB;
		else if (key == "LEVEL5_SHIFT") m_keybinds[name] = SDLK_LEVEL5_SHIFT;
		else if (key == "MULTI_KEY_COMPOSE") m_keybinds[name] = SDLK_MULTI_KEY_COMPOSE;
		else if (key == "LMETA") m_keybinds[name] = SDLK_LMETA;
		else if (key == "RMETA") m_keybinds[name] = SDLK_RMETA;
		else if (key == "LHYPER") m_keybinds[name] = SDLK_LHYPER;
		else if (key == "RHYPER") m_keybinds[name] = SDLK_RHYPER;
		else sdl.error("Invalid config!", std::format("the key {} does not exist!", key));
		// clang-format on
	}
	if (count > c_keybind_set.size()) {
		sdl.error("Invalid config!", "more than existing keybinds are defined!");
	} else if (count < c_keybind_set.size()) {
		sdl.error("Invalid config!", "not all keybinds are defined!");
	}

    file.close();
}
