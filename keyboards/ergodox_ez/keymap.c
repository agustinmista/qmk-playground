/* Copyright 2020 ZSA Technology Labs, Inc <@zsa>
 * Copyright 2020 Jack Humbert <jack.humb@gmail.com>
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2023 Agustín Mista <agustin@mista.me>
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
#include "version.h"

/*
 * Layers
 */

enum keyboard_layers {
  BASE_LAYER,
  LOWER_LAYER,
  RAISE_LAYER,
  HYPER_LAYER
};

/*
 * Custom keycodes
 */

enum keyboard_keycodes {
  BASE = SAFE_RANGE,    // Set the default layer to BASE_LAYER
  LOWER,                // Set the default layer to LOWER_LAYER
  RAISE,                // Set the default layer to RAISE_LAYER
  HYPER                 // Set the default layer to HYPER_LAYER
};

/*
 * Keycode aliases
 */

// Aliases for LT() to a particular layer
#define LT_BASE(kc)  LT(BASE_LAYER,kc)
#define LT_LOWER(kc) LT(LOWER_LAYER,kc)
#define LT_RAISE(kc) LT(RAISE_LAYER,kc)
#define LT_HYPER(kc) LT(HYPER_LAYER,kc)

// Aliases for other combos
#define TASK_VW LCTL(LALT(KC_TAB))
#define VSC_MEN LCTL(LSFT(KC_P))
#define VSC_WHC MEH(KC_P)
#define MEH_SPC MEH(KC_SPC)
#define FF_PTAB LCTL(KC_PGUP)
#define FF_NTAB LCTL(KC_PGDN)
#define WIN_RUN LALT(KC_SPC)

/*
 * Initialization code
 */

void keyboard_post_init_user(void) {
  ergodox_led_all_off();
}

/*
 * Led helpers
 */

// Change the top right RGB to indicate the layer
void ergodox_led_set_color_by_layer(uint8_t layer) {
  ergodox_led_all_off();
  switch(layer) {
    case BASE_LAYER:
      break;
    case LOWER_LAYER:
      ergodox_right_led_1_on();
      break;
    case RAISE_LAYER:
      ergodox_right_led_2_on();
      break;
    case HYPER_LAYER:
      ergodox_right_led_3_on();
      break;
  }
}

// Blink the right LED 1
void ergodox_blink_right_led_1(uint8_t times) {
  for (int i=0; i<times; i++) {
    ergodox_right_led_1_on();
    _delay_ms(50);
    ergodox_right_led_1_off();
    _delay_ms(50);
  }
}

// Blink the right LED 2
void ergodox_blink_right_led_2(uint8_t times) {
  for (int i=0; i<times; i++) {
    ergodox_right_led_2_on();
    _delay_ms(50);
    ergodox_right_led_2_off();
    _delay_ms(50);
  }
}

// Blink the right LED 3
void ergodox_blink_right_led_3(uint8_t times) {
  for (int i=0; i<times; i++) {
    ergodox_right_led_2_on();
    _delay_ms(50);
    ergodox_right_led_2_off();
    _delay_ms(50);
  }
}

/*
 * Leader mode
 */

bool leader_mode = false;

// Macros to trigger standard compose key sequences
#define COMPOSE_KEY      SS_TAP(X_RALT)
#define LOWER_ACUTE(kc)  SS_TAP(X_QUOT) SS_TAP(kc)
#define UPPER_ACUTE(kc)  SS_TAP(X_QUOT) SS_LSFT(SS_TAP(kc))
#define LOWER_UMLAUT(kc) SS_LSFT(SS_TAP(X_QUOT)) SS_TAP(kc)
#define UPPER_UMLAUT(kc) SS_LSFT(SS_TAP(X_QUOT)) SS_LSFT(SS_TAP(kc))

// Macros for registering key sequences
#define ONE_KEY_SEQUENCE(kc1, str) \
  if (leader_sequence_one_key(kc1)) { SEND_STRING(str); return true; }
#define TWO_KEYS_SEQUENCE(kc1, kc2, str) \
  if (leader_sequence_two_keys(kc1, kc2)) { SEND_STRING(str); return true; }
#define THREE_KEYS_SEQUENCE(kc1, kc2, kc3, str) \
  if (leader_sequence_three_keys(kc1, kc2, kc3)) { SEND_STRING(str); return true; }

// The dictionary of sequences
bool process_leader_sequence(void) {
  // leader + t ==> Ctrl+Shift+t
  ONE_KEY_SEQUENCE(KC_T, SS_LCTL(SS_LSFT(SS_TAP(X_T))));
  // leader + q ==> Alt+F4
  ONE_KEY_SEQUENCE(KC_Q, SS_LALT(SS_TAP(X_F4)));
  // leader + s + s ==> Sleep
  TWO_KEYS_SEQUENCE(KC_S, KC_S, SS_TAP(X_PWR));
  // Lowercase acutes
  // E.g. leader + a ==> Right Alt+'+a ==> á
  ONE_KEY_SEQUENCE(KC_A, COMPOSE_KEY LOWER_ACUTE(X_A));
  ONE_KEY_SEQUENCE(KC_E, COMPOSE_KEY LOWER_ACUTE(X_E));
  ONE_KEY_SEQUENCE(KC_I, COMPOSE_KEY LOWER_ACUTE(X_I));
  ONE_KEY_SEQUENCE(KC_O, COMPOSE_KEY LOWER_ACUTE(X_O));
  ONE_KEY_SEQUENCE(KC_U, COMPOSE_KEY LOWER_ACUTE(X_U));
  // Uppercase acutes
  // E.g. leader + Left Shift + a ==> Right Alt+'+A ==> Á
  TWO_KEYS_SEQUENCE(KC_LSFT, KC_A, COMPOSE_KEY UPPER_ACUTE(X_A));
  TWO_KEYS_SEQUENCE(KC_LSFT, KC_E, COMPOSE_KEY UPPER_ACUTE(X_E));
  TWO_KEYS_SEQUENCE(KC_LSFT, KC_I, COMPOSE_KEY UPPER_ACUTE(X_I));
  TWO_KEYS_SEQUENCE(KC_LSFT, KC_O, COMPOSE_KEY UPPER_ACUTE(X_O));
  TWO_KEYS_SEQUENCE(KC_LSFT, KC_U, COMPOSE_KEY UPPER_ACUTE(X_U));
  // Ñ letter
  ONE_KEY_SEQUENCE(KC_N, COMPOSE_KEY SS_LSFT(SS_TAP(X_GRAVE)) SS_TAP(X_N));
  TWO_KEYS_SEQUENCE(KC_LSFT, KC_N, COMPOSE_KEY SS_LSFT(SS_TAP(X_GRAVE)) SS_LSFT(SS_TAP(X_N)));
  // Swedish letters
  TWO_KEYS_SEQUENCE(KC_A, KC_E, COMPOSE_KEY LOWER_UMLAUT(X_A));
  TWO_KEYS_SEQUENCE(KC_O, KC_E, COMPOSE_KEY LOWER_UMLAUT(X_O));
  TWO_KEYS_SEQUENCE(KC_O, KC_A, COMPOSE_KEY SS_TAP(X_O) SS_TAP(X_A))
  THREE_KEYS_SEQUENCE(KC_LSFT, KC_A, KC_E, COMPOSE_KEY UPPER_UMLAUT(X_A));
  THREE_KEYS_SEQUENCE(KC_LSFT, KC_O, KC_E, COMPOSE_KEY UPPER_UMLAUT(X_O));
  THREE_KEYS_SEQUENCE(KC_LSFT, KC_O, KC_A, COMPOSE_KEY SS_TAP(X_O) SS_LSFT(SS_TAP(X_A)));

  return false;
}

// Start leader mode hook
void leader_start_user(void) {
  ergodox_led_all_on();
  leader_mode = true;
}

// End leader mode hook
void leader_end_user(void) {
  bool success = process_leader_sequence();
  ergodox_led_all_off();
  if (success) {
    ergodox_blink_right_led_2(2);
  } else {
    ergodox_blink_right_led_1(2);
  }
  leader_mode = false;
}

/*
 * Sticky layers
 */

uint16_t sticky_layer = BASE_LAYER;

// Like set_single_persistent_default_layer but without writing to EEPROM
void set_single_default_layer(uint16_t layer) {
  default_layer_set((layer_state_t)1 << layer);
}

// Set a given layer or revert to the base one if the given layer is already set
void set_or_revert_default_layer(uint16_t layer) {
  if (sticky_layer == layer) { // The layer is already set, move to the base layer
    sticky_layer = BASE_LAYER;
    set_single_default_layer(BASE_LAYER);
    ergodox_led_set_color_by_layer(BASE_LAYER);
  } else { // The layer is not set, move to it
    sticky_layer = layer;
    set_single_default_layer(layer);
  }
}

/*
 * Process custom keycodes
 */

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    // Sticky mode keycodes
    case LOWER:
      if (record->event.pressed) {
        set_or_revert_default_layer(LOWER_LAYER);
      }
      return false;
    case RAISE:
      if (record->event.pressed) {
        set_or_revert_default_layer(RAISE_LAYER);
      }
      return false;
    case HYPER:
      if (record->event.pressed) {
        set_or_revert_default_layer(HYPER_LAYER);
      }
      return false;
  }

  return true;
};

/*
 * Change the top right LEDs based on layer
 */

layer_state_t layer_state_set_user(layer_state_t state) {

  // Do not repaint if we are in a mode that overrides the default layer color
  if (leader_mode) {
    return state;
  }

  // Set the underglow to the corresponding layer color
  switch (get_highest_layer(state | default_layer_state)) {
    case LOWER_LAYER:
      ergodox_led_set_color_by_layer(LOWER_LAYER);
      break;
    case RAISE_LAYER:
      ergodox_led_set_color_by_layer(RAISE_LAYER);
      break;
    case HYPER_LAYER:
      ergodox_led_set_color_by_layer(HYPER_LAYER);
      break;
    default:
      ergodox_led_all_off();
      break;
  }

  return state;
}

/*
 * Keymaps
 */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[BASE_LAYER] = LAYOUT_ergodox(
  // Left hand
  KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_MUTE,
  KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_VOLU,
  KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,
  KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_VOLD,
  KC_LCTL, KC_LCTL, XXXXXXX, XXXXXXX, KC_LCBR,
                                               XXXXXXX, XXXXXXX,
                                                        QK_LEAD,
                             LT_LOWER(KC_SPC), KC_LALT, WIN_RUN,

  // Right hand
  KC_MPLY, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  KC_MNXT, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
           KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  KC_MPRV, KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
                    KC_RCBR, XXXXXXX, XXXXXXX, KC_RCTL, KC_RCTL,
  XXXXXXX, XXXXXXX,
  QK_LEAD,
  VSC_MEN, VSC_WHC, LT_RAISE(KC_ENT)
),

[LOWER_LAYER] = LAYOUT_ergodox(
  // Left hand
  KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, _______,
  MEH_SPC, KC_F1,   KC_F2,   KC_F3,   KC_F4,   XXXXXXX, _______,
  TASK_VW, KC_F5,   KC_F6,   KC_F7,   KC_F8,   XXXXXXX,
  _______, KC_F9,   KC_F10,  KC_F11,  KC_F12,  XXXXXXX, _______,
  _______, _______, XXXXXXX, XXXXXXX, KC_LBRC,
                                               XXXXXXX, XXXXXXX,
                                                        LOWER,
                                      _______, _______, _______,

  // Right hand
  _______, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_DEL,
  _______, XXXXXXX, KC_UNDS, KC_PLUS, XXXXXXX, XXXXXXX, KC_PIPE,
           XXXXXXX, KC_MINS, KC_EQL,  XXXXXXX, KC_COLN, KC_DQUO,
  _______, XXXXXXX, XXXXXXX, KC_LABK, KC_RABK, KC_QUES, _______,
                    KC_RBRC, XXXXXXX, XXXXXXX, _______, _______,
  XXXXXXX, XXXXXXX,
  LOWER,
  _______, _______, LT_HYPER(KC_BSPC)
),

[RAISE_LAYER] = LAYOUT_ergodox(
  // Left hand
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,
  MEH_SPC, XXXXXXX, KC_BTN2, KC_MS_U, KC_BTN1, XXXXXXX, _______,
  TASK_VW, XXXXXXX, KC_MS_L, KC_MS_D, KC_MS_R, XXXXXXX,
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,
  _______, _______, XXXXXXX, XXXXXXX, KC_LBRC,
                                               XXXXXXX, XXXXXXX,
                                                        RAISE,
                             LT_HYPER(KC_DEL), _______, _______,

  // Right hand
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  _______, XXXXXXX, FF_PTAB, KC_UP,   FF_NTAB, XXXXXXX, XXXXXXX,
           XXXXXXX, KC_LEFT, KC_DOWN, KC_RGHT, XXXXXXX, XXXXXXX,
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,
                    KC_RBRC, XXXXXXX, XXXXXXX, _______, _______,
  XXXXXXX, XXXXXXX,
  RAISE,
  _______, _______, _______
),

[HYPER_LAYER] = LAYOUT_ergodox(
  // Left hand
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  _______, KC_F13,  KC_F14,  KC_F15,  KC_F16,  XXXXXXX, XXXXXXX,
  _______, KC_F17,  KC_F18,  KC_F19,  KC_F20,  XXXXXXX,
  _______, KC_F21,  KC_F22,  KC_F23,  KC_F24,  XXXXXXX, XXXXXXX,
  _______, _______, XXXXXXX, XXXXXXX, KC_LABK,
                                               XXXXXXX, XXXXXXX,
                                                        HYPER,
                                      _______, _______, _______,

  // Right hand
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, QK_BOOT,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,
                    KC_RABK, XXXXXXX, XXXXXXX, _______, _______,
  XXXXXXX, XXXXXXX,
  HYPER,
  _______, _______, _______
)};