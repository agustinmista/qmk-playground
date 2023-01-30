/* Copyright 2015-2021 Jack Humbert
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

#include QMK_KEYBOARD_H

/*
 * Layers
 */

enum preonic_layers {
  _BASE,
  _LOWER,
  _RAISE,
  _ADJUST
};

// Aliases for LT() to a particular layer
#define LT_BSE(kc) LT(_BASE,kc)
#define LT_LWR(kc) LT(_LOWER,kc)
#define LT_RSE(kc) LT(_RAISE,kc)
#define LT_ADJ(kc) LT(_ADJUST,kc)

/*
 * Custom keycodes
 */

enum preonic_keycodes {
  BASE = SAFE_RANGE,    // Set the default layer to _BASE
  LOWER,                // Set the default layer to _LOWER
  RAISE,                // Set the default layer to _RAISE
  ADJUST                // Set the default layer to _ADJUST
};

/*
 * Keymaps
 */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_BASE] = LAYOUT_preonic_2x2u(
  KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
  KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, OSM(MOD_LSFT),
  KC_LCTL, KC_LGUI, BASE,    KC_LALT,  LT_LWR(KC_SPC),   LT_RSE(KC_ENT),  KC_RALT, KC_LEAD, KC_DOWN, KC_UP
),

[_LOWER] = LAYOUT_preonic_2x2u(
  KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_DEL,
  KC_F13,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   XXXXXXX, XXXXXXX, KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE,
  KC_F14,  KC_F5,   KC_F6,   KC_F7,   KC_F8,   XXXXXXX, XXXXXXX, KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_DQUO,
  _______, KC_F9,   KC_F10,  KC_F11,  KC_F12,  XXXXXXX, XXXXXXX, XXXXXXX, KC_LABK, KC_RABK, KC_QUES, OSM(MOD_LSFT),
  _______, _______, LOWER,   _______,     _______,       LT_ADJ(KC_ENT),  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
),

[_RAISE] = LAYOUT_preonic_2x2u(
  KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_DEL,
  XXXXXXX, XXXXXXX, KC_BTN2, KC_MS_U, KC_BTN1, XXXXXXX, XXXXXXX, KC_VOLD, KC_UP,   KC_VOLU, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, KC_MS_L, KC_MS_D, KC_MS_R, XXXXXXX, XXXXXXX, KC_LEFT, KC_DOWN, KC_RGHT, XXXXXXX, XXXXXXX,
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_MPRV, KC_MPLY, KC_MNXT, XXXXXXX, XXXXXXX,
  _______, _______, RAISE,   _______,  LT_ADJ(KC_SPC),      _______,      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
),

[_ADJUST] = LAYOUT_preonic_2x2u(
  XXXXXXX, QK_BOOT, QK_RBT,  EE_CLR,  DB_TOGG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, RGB_TOG, RGB_MOD, RGB_HUD, RGB_HUI, RGB_SAD, RGB_SAI, RGB_VAD, RGB_VAI, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, ADJUST,  XXXXXXX,     _______,          _______,      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
)

};

/*
 * Initialization code
 */

void keyboard_post_init_user(void) {
  set_single_persistent_default_layer(_BASE);
}

/*
 * Process layer modifiers
 */

void set_or_revert_default_layer(uint8_t layer) {
  if (IS_LAYER_ON(layer)) {
    set_single_persistent_default_layer(layer);
  } else {
    set_single_persistent_default_layer(_BASE);
    layer_move(_BASE);
  }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case LOWER:
      if (record->event.pressed) {
        set_or_revert_default_layer(_LOWER);
      }
      return false;
    case RAISE:
      if (record->event.pressed) {
        set_or_revert_default_layer(_RAISE);
      }
      return false;
    case ADJUST:
      if (record->event.pressed) {
        set_or_revert_default_layer(_ADJUST);
      }
      return false;
  }
  return true;
};

/*
 * Leader key sequences
 */

LEADER_EXTERNS();

void matrix_scan_user(void) {
  LEADER_DICTIONARY() {
    leading = false;
    leader_end();

    SEQ_ONE_KEY(KC_T) {
      SEND_STRING(SS_LCTL(SS_LSFT("t")));
    }

    SEQ_TWO_KEYS(KC_Q, KC_Q) {
      SEND_STRING(SS_LALT(SS_TAP(X_F4)));
    }

  }
}

/*
 * Change the color depending on layer
 */

layer_state_t layer_state_set_user(layer_state_t state) {
  switch (get_highest_layer(state | default_layer_state)) {
  case _LOWER:
    rgblight_sethsv_noeeprom(HSV_GREEN);
    break;
  case _RAISE:
    rgblight_sethsv_noeeprom(HSV_TEAL);
    break;
  case _ADJUST:
    rgblight_sethsv_noeeprom(HSV_PURPLE);
    break;
  default:
    rgblight_sethsv_noeeprom(HSV_RED);
    break;
  }
  return state;
}

uint8_t old_hue;
uint8_t old_sat;
uint8_t old_val;

void leader_start(void) {
  old_hue = rgblight_get_hue();
  old_sat = rgblight_get_sat();
  old_val = rgblight_get_val();
  rgblight_sethsv_noeeprom(HSV_WHITE);
}

void leader_end(void) {
  rgblight_sethsv_noeeprom(old_hue, old_sat, old_val);
}
