#pragma once

#include <SDL3/SDL.h>

constexpr Uint32 ck_escape = SDLK_ESCAPE;
constexpr Uint32 ck_copy = SDLK_C;
constexpr Uint32 ck_paste = SDLK_V;
constexpr Uint32 ck_cut = SDLK_X;
constexpr Uint32 ck_undo = SDLK_Z;
constexpr Uint32 ck_options = SDLK_LALT;

constexpr Uint32 ck_home_pos = SDLK_B;

constexpr Uint32 ck_equalize_horizontal_distance = SDLK_H;
constexpr Uint32 ck_straighten_selection = SDLK_S;
constexpr Uint32 ck_align_vertically = SDLK_W;

constexpr Uint32 ck_delete_gate = SDLK_X;
constexpr Uint32 ck_add_gate = SDLK_A;
constexpr Uint32 ck_duplicate_gate = SDLK_D;
constexpr Uint32 ck_package_gates = SDLK_T;
constexpr Uint32 ck_edit_packaged_gate = SDLK_E;
constexpr Uint32 ck_predict_node = SDLK_LCTRL;
constexpr Uint32 ck_align_node_line_with_axis = SDLK_LSHIFT;

constexpr Uint32 ck_quick_access_0 = SDLK_1;
constexpr Uint32 ck_quick_access_1 = SDLK_2;
constexpr Uint32 ck_quick_access_2 = SDLK_3;
constexpr Uint32 ck_quick_access_3 = SDLK_4;
constexpr Uint32 ck_quick_access_4 = SDLK_5;
constexpr Uint32 ck_quick_access_5 = SDLK_6;

// keymap:
//
// |               | QA0      | QA1        | QA2           | QA3            | QA4          |
// | Connect close |          | AlignV     | Edit Packaged |                | Package      |
// | Esc           | Add gate | Straighten | Duplicate     |                |              |
// | Predict Node  | Undo     | Delete     | Copy          | Paste          | Home         |
//                            | Options    |               | Straighen line | Predict Node |
