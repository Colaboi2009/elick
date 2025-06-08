#pragma once

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

class KeybindParser {
  private:
    const std::string c_config_filepath = "elric.conf";
    const std::unordered_set<std::string> c_keybind_set = {
        "escape",
        "return",
        "copy",
        "paste",
        "cut",
        "undo",
        "options",
        "backspace",
        "down",
        "up",

        "home_pos",
        "equalize_horizontal_distance",
        "straighten_selection",
        "align_vertically",
        "delete_gate",
        "add_gate",
        "package_gates",
        "edit_packaged_gate",
        "predict_node",
        "align_node_line_with_axis",

        "quick_access_0",
        "quick_access_1",
        "quick_access_2",
        "quick_access_3",
        "quick_access_4",

        "packager_name",
        "packager_red",
        "packager_green",
        "packager_blue",
        "packager_accept",
        "packager_clear_inputs",
        "packager_randomize_colors",
    };

    std::unordered_map<std::string, SDL_Keycode> m_keybinds;

  public:
    KeybindParser();
    ~KeybindParser();

    SDL_Keycode get(std::string name);
    bool pressed(SDL_Event &e, std::string name);

    void loadKeybinds();
};
