/* Copyright 2019 Thomas Baart <thomas@splitkb.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "action_util.h"
#include "keycode.h"
#include "keycodes.h"
#include <string.h>
#pragma GCC diagnostic ignored "-Wattributes"
#include "quantum.h"
#include QMK_KEYBOARD_H
#include "timer.h"
#include "version.h"
#ifdef CONSOLE_ENABLE
#include "print.h"
#endif

#include "encoder.h"
#include "pgherveou.h"

__attribute__((weak)) bool process_record_secrets(uint16_t keycode,
                                                  keyrecord_t *record) {
  return true;
}

static uint16_t retro_timer;
static bool nav_caps_on = false;

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* ┌──────┬──────┬──────┬──────┬──────┬──────┐                              ┌──────┬──────┬──────┬──────┬──────┬──────┐
 * │CTL_S_│  Q   │  W   │  E   │  R   │  T   │                              │  Y   │  U   │  I   │  O   │  P   │ = +  │
 * ├──────┼──────┼──────┼──────┼──────┼──────┤                              ├──────┼──────┼──────┼──────┼──────┼──────┤
 * │CTL_TA│  A   │  S   │  D   │  F   │  G   │                              │  H   │  J   │  K   │  L   │ ; :  │QUOT_C│
 * ├──────┼──────┼──────┼──────┼──────┼──────┼──────┬──────┐  ┌──────┬──────┼──────┼──────┼──────┼──────┼──────┼──────┤
 * │ HYPR │  Z   │  X   │  C   │  V   │  B   │  [   │ MEH  │  │FKEYS │  ]   │  N   │  M   │ , >  │ . <  │ / ?  │ \ |  │
 * └──────┴──────┴──────┼──────┼──────┼──────┼──────┼──────┤  ├──────┼──────┼──────┼──────┼──────┼──────┴──────┴──────┘
 *                      │ENC_L │ LALT │SFT_CO│SPC_NA│GUI_DE│  │CTL_BS│ENT_SY│SFT_MI│ RGUI │ENC_R │
 *                      │      │      │      │      │      │  │      │      │      │      │      │
 *                      └──────┴──────┴──────┴──────┴──────┘  └──────┴──────┴──────┴──────┴──────┘
 *                                                                               generated by [keymapviz] */
  [_QWERTY] = LAYOUT(
     CTL_S_TILT  , KC_Q ,  KC_W   ,  KC_E  ,   KC_R ,   KC_T,                                          KC_Y,   KC_U ,  KC_I ,   KC_O ,  KC_P , KC_EQL,
     CTL_TAB , KC_A ,  KC_S   ,  KC_D  ,  KC_F  ,   KC_G,                                          KC_H,   KC_J ,  KC_K ,   KC_L ,KC_SCLN, QUOT_CMD,
     KC_HYPR,  KC_Z ,  KC_X   ,  KC_C  ,   KC_V ,   KC_B , KC_LBRC, KC_MEH,      FKEYS  , KC_RBRC,  KC_N,   KC_M ,KC_COMM, KC_DOT ,KC_SLSH, KC_BSLS,
                               ENC_L   , KC_LALT, SFT_COL, SPC_NAV, GUI_DEL,     GUI_BSP, ENT_SYM, SFT_MIN, KC_RGUI,ENC_R
    ),
/* ┌──────┬──────┬──────┬──────┬──────┬──────┐                              ┌──────┬──────┬──────┬──────┬──────┬──────┐
 * │      │      │WORD_R│      │S(UNDO│      │                              │ HOME │ END  │ PGUP │NAV_O │PASTE │SCREEN│
 * ├──────┼──────┼──────┼──────┼──────┼──────┤                              ├──────┼──────┼──────┼──────┼──────┼──────┤
 * │SELECT│ LCTL │ LGUI │ LALT │ LSFT │      │                              │ LEFT │ DOWN │  UP  │ RGHT │      │      │
 * ├──────┼──────┼──────┼──────┼──────┼──────┼──────┬──────┐  ┌──────┬──────┼──────┼──────┼──────┼──────┼──────┼──────┤
 * │C(KC_B│ UNDO │ CUT  │ COPY │NAV_V │WORD_L│      │      │  │      │      │      │ PGDN │      │      │      │      │
 * └──────┴──────┴──────┼──────┼──────┼──────┼──────┼──────┤  ├──────┼──────┼──────┼──────┼──────┼──────┴──────┴──────┘
 *                      │      │      │      │      │      │  │S(WORD│NAV_CA│S(WORD│      │      │
 *                      │      │      │      │      │      │  │      │      │      │      │      │
 *                      └──────┴──────┴──────┴──────┴──────┘  └──────┴──────┴──────┴──────┴──────┘
 *                                                                               generated by [keymapviz] */
  [_NAV] = LAYOUT(
      XXXXXXX, XXXXXXX, WORD_RIGHT, XXXXXXX,S(UNDO),XXXXXXX,                                           KC_HOME, KC_END,  KC_PGUP, NAV_O,  PASTE, SCREENSHOT,
      SELECT_ALL, KC_LCTL, KC_LGUI, KC_LALT, KC_LSFT, XXXXXXX,                                         KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, XXXXXXX, XXXXXXX,
      C(KC_B), UNDO, CUT, COPY, NAV_V,  WORD_LEFT, XXXXXXX, XXXXXXX,        XXXXXXX, XXXXXXX, XXXXXXX, KC_PGDN, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                 KC_TRNS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,     S(WORD_LEFT), NAV_CAPS, S(WORD_RIGHT), _______,KC_TRNS
    ),
/* ┌──────┬──────┬──────┬──────┬──────┬──────┐                              ┌──────┬──────┬──────┬──────┬──────┬──────┐
 * │ ` ~  │  1   │  2   │  3   │  4   │  5   │                              │  6   │  7   │  8   │  9   │TERM_P│  +   │
 * ├──────┼──────┼──────┼──────┼──────┼──────┤                              ├──────┼──────┼──────┼──────┼──────┼──────┤
 * │  ~   │  !   │  @   │  #   │  $   │  %   │                              │  ^   │GUI_AM│SFT_AS│  (   │  )   │RIGHT │
 * ├──────┼──────┼──────┼──────┼──────┼──────┼──────┬──────┐  ┌──────┬──────┼──────┼──────┼──────┼──────┼──────┼──────┤
 * │  |   │ \ |  │  [   │  {   │  (   │ = +  │  <   │      │  │      │  >   │  +   │  )   │  }   │  ]   │ / ?  │  ?   │
 * └──────┴──────┴──────┼──────┼──────┼──────┼──────┼──────┤  ├──────┼──────┼──────┼──────┼──────┼──────┴──────┴──────┘
 *                      │      │      │ . <  │  0   │ , >  │  │      │      │      │      │      │
 *                      │      │      │      │      │      │  │      │      │      │      │      │
 *                      └──────┴──────┴──────┴──────┴──────┘  └──────┴──────┴──────┴──────┴──────┘
 *                                                                               generated by [keymapviz] */
  [_SYM] = LAYOUT(
      KC_GRV ,   KC_1 ,   KC_2 ,   KC_3 ,   KC_4 ,   KC_5 ,                                            KC_6 ,   KC_7 ,   KC_8 ,   KC_9 , TERM_PASTE, KC_PLUS,
     KC_TILD , KC_EXLM,  KC_AT , KC_HASH,  KC_DLR, KC_PERC,                                          KC_CIRC, GUI_AMPR, SFT_ASTR, KC_LPRN, KC_RPRN, KC_RIGHT,
     KC_PIPE , KC_BSLS, KC_LBRC, KC_LCBR, KC_LPRN, KC_EQL, KC_LT, XXXXXXX,         XXXXXXX, KC_GT  , KC_PLUS, KC_RPRN, KC_RCBR, KC_RBRC, KC_SLSH, KC_QUES,
                              KC_TRNS, _______, KC_DOT, KC_0,  KC_COMM,        _______, XXXXXXX, XXXXXXX, XXXXXXX, KC_TRNS
    ),
/* ┌──────┬──────┬──────┬──────┬──────┬──────┐                              ┌──────┬──────┬──────┬──────┬──────┬──────┐
 * │G(KC_0│G(KC_1│G(KC_2│G(KC_3│G(KC_4│G(KC_5│                              │G(KC_6│G(KC_7│G(KC_8│G(KC_9│      │      │
 * ├──────┼──────┼──────┼──────┼──────┼──────┤                              ├──────┼──────┼──────┼──────┼──────┼──────┤
 * │      │      │      │      │      │      │                              │      │      │      │      │ RSFT │      │
 * ├──────┼──────┼──────┼──────┼──────┼──────┼──────┬──────┐  ┌──────┬──────┼──────┼──────┼──────┼──────┼──────┼──────┤
 * │      │      │      │      │      │      │      │      │  │      │      │      │      │      │      │      │      │
 * └──────┴──────┴──────┼──────┼──────┼──────┼──────┼──────┤  ├──────┼──────┼──────┼──────┼──────┼──────┴──────┴──────┘
 *                      │      │      │      │      │      │  │      │      │      │      │      │
 *                      │      │      │      │      │      │  │      │      │      │      │      │
 *                      └──────┴──────┴──────┴──────┴──────┘  └──────┴──────┴──────┴──────┴──────┘
 *                                                                               generated by [keymapviz] */
  [_CMD] = LAYOUT(
     G(KC_0) , G(KC_1),  G(KC_2) , G(KC_3),  G(KC_4), G(KC_5),                         G(KC_6), G(KC_7), G(KC_8), G(KC_9),_______, _______,
     _______ , _______,  _______ , _______,  _______, _______,                                          _______, _______, _______, _______, KC_RSFT, _______,
     _______, _______, _______, _______, _______, _______, _______, _______,         _______, _______, _______, _______, _______, _______, _______, _______,
                              _______, _______, _______, _______,  _______,        _______, _______, _______, _______, _______ 
    ),
/* ┌──────┬──────┬──────┬──────┬──────┬──────┐                              ┌──────┬──────┬──────┬──────┬──────┬──────┐
 * │      │SECRET│SECRET│SECRET│      │      │                              │      │      │      │      │      │      │
 * ├──────┼──────┼──────┼──────┼──────┼──────┤                              ├──────┼──────┼──────┼──────┼──────┼──────┤
 * │G(C(KC│  F1  │  F2  │  F3  │  F4  │  F5  │                              │      │ RSFT │ RCTL │ LALT │ RGUI │      │
 * ├──────┼──────┼──────┼──────┼──────┼──────┼──────┬──────┐  ┌──────┬──────┼──────┼──────┼──────┼──────┼──────┼──────┤
 * │      │  F6  │  F7  │  F8  │  F9  │ F10  │ F11  │ F12  │  │      │      │      │      │      │      │      │      │
 * └──────┴──────┴──────┼──────┼──────┼──────┼──────┼──────┤  ├──────┼──────┼──────┼──────┼──────┼──────┴──────┴──────┘
 *                      │      │      │      │      │      │  │      │      │      │      │      │
 *                      │      │      │      │      │      │  │      │      │      │      │      │
 *                      └──────┴──────┴──────┴──────┴──────┘  └──────┴──────┴──────┴──────┴──────┘
 *                                                                               generated by [keymapviz] */
  [_FUNCTION] = LAYOUT(
      XXXXXXX, KC_SECRET_1,  KC_SECRET_2,  KC_SECRET_3,  XXXXXXX, XXXXXXX,                                         XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
      G(C(KC_Q)),  KC_F1  ,  KC_F2,   KC_F3 ,  KC_F4 ,  KC_F5,                                  XXXXXXX, KC_RSFT, KC_RCTL, KC_LALT, KC_RGUI, XXXXXXX,
      XXXXXXX,  KC_F6  ,  KC_F7 ,  KC_F8 ,  KC_F9 ,  KC_F10, KC_F11, KC_F12,        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                 KC_TRNS,  XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_TRNS
    ),
};

// clang-format on

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (!process_record_encoder(keycode, record)) {
    return false;
  }

  if (!process_record_secrets(keycode, record)) {
    return false;
  }

  if (keycode == SPC_NAV && nav_caps_on && !record->event.pressed) {
    nav_caps_on = false;
    unregister_code16(KC_LSFT);
  }

  // work around MT limitation to send shifted keycode (:)
  // see
  // https://beta.docs.qmk.fm/using-qmk/advanced-keycodes/mod_tap#changing-tap-function
  if (keycode == SFT_COL && record->event.pressed && record->tap.count >= 1) {
    tap_code16(KC_COLN);
    return false;
  }
  if (keycode == GUI_AMPR && record->event.pressed && record->tap.count >= 1) {
    tap_code16(KC_AMPR);
    return false;
  }
  if (keycode == SFT_ASTR && record->event.pressed && record->tap.count >= 1) {
    tap_code16(KC_ASTR);
    return false;
  }

  uint8_t mod_state = get_mods();
  if ((MOD_BIT(KC_LEFT_ALT) == mod_state) && record->event.pressed) {
    if (keycode == KC_H) {
      unregister_code16(KC_LEFT_ALT);
      tap_code16(KC_ESC);
      tap_code16(KC_B);
      register_code16(KC_LEFT_ALT);
      return false;
    } else if (keycode == KC_L) {
      unregister_code16(KC_LEFT_ALT);
      tap_code16(KC_ESC);
      tap_code16(KC_F);
      register_code16(KC_LEFT_ALT);
      return false;
    }
  }

  switch (keycode) {
  case SPC_NAV:
  case QUOT_CMD:
  case HYP_A:
  case CTL_TAB:
  case SFT_COL:
  case SFT_MIN:
  case ENT_SYM:
    if (record->event.pressed) {
      retro_timer = timer_read();
    }
    break;
  case NAV_V:
    if (record->event.pressed) {
      SEND_STRING(SS_TAP(X_HOME) SS_LSFT(SS_TAP(X_END)));
      register_code16(KC_LSFT);
    } else {
      unregister_code16(KC_LSFT);
    }
    break;
  case NAV_CAPS:
    if (record->event.pressed) {
      if (nav_caps_on) {
        unregister_code16(KC_LSFT);
      } else {
        register_code16(KC_LSFT);
      }
      nav_caps_on = !nav_caps_on;
    }
    break;
  case NAV_O:
    if (record->event.pressed) {
      unregister_mods(MOD_MASK_SHIFT);
      mod_state &MOD_MASK_SHIFT
          ? SEND_STRING(SS_TAP(X_HOME) SS_TAP(X_ENTER) SS_TAP(X_UP))
          : SEND_STRING(SS_TAP(X_END) SS_TAP(X_ENTER));
      set_mods(mod_state);
    }
    break;
  }
  return true;
};

#ifdef PERMISSIVE_HOLD_PER_KEY
bool get_permissive_hold(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
  case CTL_TAB:
    // Immediately select the hold action when another key is tapped.
    return true;
  default:
    // Do not select the hold action when another key is tapped.
    return false;
  }
}
#endif

#ifdef TAPPING_TERM_PER_KEY
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
  case CTL_TAB:
    // return TAPPING_TERM  if any mods is pressed
    if (get_mods() != 0) {
      return TAPPING_TERM;
    }
    return 100;
  default:
    return TAPPING_TERM;
  }
}
#endif

#ifdef RETRO_TAPPING_PER_KEY
bool get_retro_tapping(uint16_t keycode, keyrecord_t *record) {
  // enable retro tapping only if it happens within max 2*term
  if (timer_elapsed(retro_timer) > 2 * TAPPING_TERM) {
    return false;
  }
  switch (keycode) {
  case SPC_NAV:
  case QUOT_CMD:
  case ENT_SYM:
  case SFT_MIN:
  case CTL_TAB:
  case HYP_A:
    /* case SFT_COL: */
    return true;
  default:
    return false;
  }
}
#endif

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  return OLED_ROTATION_180;
}

bool oled_task_user(void) {
  if (is_keyboard_master()) {
    // Host Keyboard Layer Status
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state | default_layer_state)) {
    case _QWERTY:
      oled_write_ln_P(PSTR("QWERTY"), false);
      break;
    case _NAV:
      oled_write_ln_P(PSTR("Nav"), false);
      break;
    case _SYM:
      oled_write_ln_P(PSTR("Sym"), false);
      break;
    case _FUNCTION:
      oled_write_ln_P(PSTR("Function"), false);
      break;
    default:
      oled_write_ln_P(PSTR("Undefined"), false);
    }

    oled_write_P(PSTR("Encoder: "), false);
    oled_write(get_enc_str(), false);
    oled_write_P(PSTR("\n"), false);

    uint8_t mods = get_mods() | get_weak_mods();
    oled_write_P((mods & MOD_MASK_GUI) ? PSTR("GUI ") : PSTR("    "), false);
    oled_write_P((mods & MOD_MASK_ALT) ? PSTR("ALT ") : PSTR("    "), false);
    oled_write_P((mods & MOD_MASK_CTRL) ? PSTR("CTRL ") : PSTR("     "), false);
    oled_write_P((mods & MOD_MASK_SHIFT) ? PSTR("SHFT ") : PSTR("     "),
                 false);
    oled_write_P(PSTR("\n"), false);
  } else {
    char *version = strtok(QMK_VERSION, "-");
    oled_write(version, false);
  }

  return false;
}
#endif

#ifdef RGBLIGHT_LAYERS
const rgblight_segment_t PROGMEM my_qwerty_layer[] =
    RGBLIGHT_LAYER_SEGMENTS({0, 20, HSV_OFF});
const rgblight_segment_t PROGMEM my_nav_layer[] =
    RGBLIGHT_LAYER_SEGMENTS({0, 20, HSV_AZURE});
const rgblight_segment_t PROGMEM my_sym_layer[] =
    RGBLIGHT_LAYER_SEGMENTS({0, 20, HSV_RED});
const rgblight_segment_t PROGMEM my_shift_layer[] =
    RGBLIGHT_LAYER_SEGMENTS({0, 20, HSV_GREEN});
const rgblight_segment_t PROGMEM my_ctrl_layer[] =
    RGBLIGHT_LAYER_SEGMENTS({4, 1, HSV_ORANGE}, {14, 1, HSV_ORANGE});
const rgblight_segment_t PROGMEM my_alt_layer[] =
    RGBLIGHT_LAYER_SEGMENTS({5, 1, HSV_GOLD}, {15, 1, HSV_GOLD});
const rgblight_segment_t PROGMEM my_gui_layer[] =
    RGBLIGHT_LAYER_SEGMENTS({9, 1, HSV_PINK}, {19, 1, HSV_PINK});
const rgblight_segment_t PROGMEM my_fn_layer[] =
    RGBLIGHT_LAYER_SEGMENTS({0, 20, HSV_PURPLE});

const rgblight_segment_t *const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    my_qwerty_layer, my_nav_layer, my_sym_layer, my_fn_layer, my_shift_layer,
    my_ctrl_layer, my_alt_layer, my_gui_layer);

layer_state_t layer_state_set_user(layer_state_t state) {
  rgblight_set_layer_state(0, layer_state_cmp(state, _QWERTY));
  rgblight_set_layer_state(1, layer_state_cmp(state, _NAV));
  rgblight_set_layer_state(2, layer_state_cmp(state, _SYM));
  rgblight_set_layer_state(3, layer_state_cmp(state, _FUNCTION));
  return state;
}

void matrix_scan_rgb(void) {
  uint8_t mods = mod_config(get_mods());
  rgblight_set_layer_state(4, mods & MOD_MASK_SHIFT);
  rgblight_set_layer_state(5, mods & MOD_MASK_CTRL);
  rgblight_set_layer_state(6, mods & MOD_MASK_ALT);
  rgblight_set_layer_state(7, mods & MOD_MASK_GUI);
}

void keyboard_post_init_user(void) {
  rgblight_layers = my_rgb_layers;
  rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
}
#endif

void matrix_init_user(void) { matrix_init_enc(); }

void matrix_scan_user(void) {
  matrix_scan_enc();
#ifdef RGBLIGHT_LAYERS
  matrix_scan_rgb();
#endif
}

#ifdef COMBO_ENABLE
const uint16_t PROGMEM COMBO_AS_KEYS[] = {KC_A, KC_S, COMBO_END};
const uint16_t PROGMEM COMBO_JK_KEYS[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM COMBO_DF_KEYS[] = {KC_D, KC_F, COMBO_END};
const uint16_t PROGMEM COMBO_CV_KEYS[] = {KC_C, KC_V, COMBO_END};

enum combo_events { COMBO_AS, COMBO_JK, COMBO_DF, COMBO_CV, COMBO_LENGTH };
uint16_t COMBO_LEN = COMBO_LENGTH;

combo_t key_combos[] = {
    [COMBO_AS] = COMBO_ACTION(COMBO_AS_KEYS),
    [COMBO_JK] = COMBO_ACTION(COMBO_JK_KEYS),
    [COMBO_DF] = COMBO_ACTION(COMBO_DF_KEYS),
    [COMBO_CV] = COMBO_ACTION(COMBO_CV_KEYS),
};

void process_combo_event(uint16_t combo_index, bool pressed) {
  switch (combo_index) {
  // as -> GUI+SHIFT+A
  case COMBO_AS:
    if (pressed) {
      tap_code16(S(CMD(KC_A)));
    }
    break;
  // jk -> escape
  case COMBO_JK:
    if (pressed) {
      tap_code16(KC_ESC);
    }
    break;
  // df -> ctrl+c
  case COMBO_DF:
    if (pressed) {
      tap_code16(C(KC_C));
    }
    break;
  // cv -> ctrl+b z
  case COMBO_CV:
    if (pressed) {
      tap_code16(C(KC_B));
      tap_code16(KC_Z);
    }
    break;
  }
}

#endif
