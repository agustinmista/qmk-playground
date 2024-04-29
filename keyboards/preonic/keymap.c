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
#include "raw_hid.h"


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
 * Layers colors
 */

#define BASE_RGB  RGB_TEAL
#define LOWER_RGB RGB_BLUE
#define RAISE_RGB RGB_GREEN
#define HYPER_RGB RGB_RED

/*
 * Custom keycodes
 */

enum keyboard_keycodes {
  BASE = SAFE_RANGE,    // Set the default layer to BASE_LAYER
  LOWER,                // Set the default layer to LOWER_LAYER
  RAISE,                // Set the default layer to RAISE_LAYER
  HYPER,                // Set the default layer to HYPER_LAYER
  REM_RGB               // Toggle remote RGB mode
};

/*
 * Keycode aliases
 */

// Aliases for LT() to a particular layer
#define LT_BASE(kc)  LT(BASE_LAYER,kc)
#define LT_LOWER(kc) LT(LOWER_LAYER,kc)
#define LT_RAISE(kc) LT(RAISE_LAYER,kc)
#define LT_HYPER(kc) LT(HYPER_LAYER,kc)

// Aliases for home row modifiers
#define HR_A    LGUI_T(KC_A)
#define HR_S    LALT_T(KC_S)
#define HR_D    LSFT_T(KC_D)
#define HR_F    LCTL_T(KC_F)

#define HR_J    LCTL_T(KC_J)
#define HR_K    LSFT_T(KC_K)
#define HR_L    LALT_T(KC_L)
#define HR_SCLN LGUI_T(KC_SCLN)

// Aliases for other combos
#define CTL_PUP LCTL(KC_PGUP)
#define CTL_PDN LCTL(KC_PGDN)
#define GUI_TAB LGUI(KC_TAB)

// Aliases for MEH
#define MEH_F1  MEH(KC_F1)
#define MEH_F2  MEH(KC_F2)
#define MEH_F3  MEH(KC_F3)
#define MEH_F4  MEH(KC_F4)
#define MEH_F5  MEH(KC_F5)
#define MEH_F6  MEH(KC_F6)
#define MEH_F7  MEH(KC_F7)
#define MEH_F8  MEH(KC_F8)
#define MEH_F9  MEH(KC_F9)
#define MEH_F10 MEH(KC_F10)
#define MEH_F11 MEH(KC_F11)
#define MEH_F12 MEH(KC_F12)

/*
 *  Songs
 */

static float sticky_on_song[][2]  = SONG(E__NOTE(_A4), E__NOTE(_A4), E__NOTE(_C5),);
static float sticky_off_song[][2] = SONG(E__NOTE(_C5), E__NOTE(_C5), E__NOTE(_A4),);

static float remote_rgb_on_song[][2]  = SONG(NUM_LOCK_ON_SOUND);
static float remote_rgb_off_song[][2] = SONG(NUM_LOCK_OFF_SOUND);

static float leader_on_song[][2] = SONG(E__NOTE(_A5), E__NOTE(_A5),);
static float leader_ok_song[][2] = SONG(E__NOTE(_A5), E__NOTE(_E6),);
static float leader_ko_song[][2] = SONG(E__NOTE(_A5), HD_NOTE(_E4),);

/*
 * Initialization code
 */

void keyboard_post_init_user(void) {
  rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
  rgblight_setrgb(BASE_RGB);
}

/*
 * RGB helpers
 */

// This is used to restore the last color when a mode interrupts another.
RGB old_rgb_val;

// Save the current RGB color
void rgblight_save_color(uint8_t r, uint8_t g, uint8_t b) {
  old_rgb_val.r = r; old_rgb_val.g = g; old_rgb_val.b = b;
}

// Restore the previous RGB color
void rgblight_restore_color(void) {
  rgblight_setrgb(old_rgb_val.r, old_rgb_val.g, old_rgb_val.b);
}

/*
 * Remote RGB mode
 */

bool remote_rgb_mode = false;

// Toggle the remote RGB mode on and off
void remote_rgb_toggle(void) {
  if(!remote_rgb_mode) {
    rgblight_save_color(RGB_YELLOW);
    rgblight_setrgb(RGB_YELLOW);
    PLAY_SONG(remote_rgb_on_song);
    remote_rgb_mode = true;
  } else {
    PLAY_SONG(remote_rgb_off_song);
    remote_rgb_mode = false;
  }
}

// Handle incoming HID messages
void raw_hid_receive(uint8_t *data, uint8_t length) {
  if (remote_rgb_mode) {
    raw_hid_send(data, length);
    uint8_t r = data[0], g = data[1], b = data[2];
    rgblight_save_color(r, g, b);
    rgblight_setrgb(r, g, b);
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
  rgblight_setrgb(RGB_WHITE);
  PLAY_SONG(leader_on_song);
  leader_mode = true;
}

// End leader mode hook
void leader_end_user(void) {
  bool success = process_leader_sequence();
  rgblight_restore_color();
  if (success) {
    PLAY_SONG(leader_ok_song);
  } else {
    PLAY_SONG(leader_ko_song);
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
    PLAY_SONG(sticky_off_song);
  } else { // The layer is not set, move to it
    sticky_layer = layer;
    set_single_default_layer(layer);
    PLAY_SONG(sticky_on_song);
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
        layer_state_set_user(layer_state);
      }
      return false;
    case RAISE:
      if (record->event.pressed) {
        set_or_revert_default_layer(RAISE_LAYER);
        layer_state_set_user(layer_state);
      }
      return false;
    case HYPER:
      if (record->event.pressed) {
        set_or_revert_default_layer(HYPER_LAYER);
        layer_state_set_user(layer_state);
      }
      return false;
    // Remote RGB mode
    case REM_RGB:
      if (record->event.pressed) {
        remote_rgb_toggle();
      }
      return false;
  }

  return true;
};

/*
 * Change specific underglow colors depending on layer
 */

layer_state_t layer_state_set_user(layer_state_t state) {

  // Do not repaint if we are in a mode that overrides the default layer color
  if (leader_mode || remote_rgb_mode) {
    return state;
  }

  // Set the underglow to the corresponding layer color
  switch (get_highest_layer(state | default_layer_state)) {
    case LOWER_LAYER:
      rgblight_save_color(LOWER_RGB);
      rgblight_setrgb(LOWER_RGB);
      break;
    case RAISE_LAYER:
      rgblight_save_color(RAISE_RGB);
      rgblight_setrgb(RAISE_RGB);
      break;
    case HYPER_LAYER:
      rgblight_save_color(HYPER_RGB);
      rgblight_setrgb(HYPER_RGB);
      break;
    default:
      rgblight_save_color(BASE_RGB);
      rgblight_setrgb(BASE_RGB);
      break;
  }

  return state;
}

/*
 * Keymaps
 */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[BASE_LAYER] = LAYOUT_preonic_2x2u(
  KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
  KC_ESC,  HR_A,    HR_S,    HR_D,    HR_F,    KC_G,    KC_H,    HR_J,    HR_K,    HR_L,    HR_SCLN, KC_QUOT,
  KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, QK_LEAD,
  MEH_F1,  MEH_F2,  MEH_F3,  KC_LCBR, LT_LOWER(KC_SPC), LT_RAISE(KC_ENT), KC_RCBR, MEH_F4,  MEH_F5,  MEH_F6
),

[LOWER_LAYER] = LAYOUT_preonic_2x2u(
  KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_DEL,
  _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   _______, _______, KC_UNDS, KC_PLUS, _______, _______, KC_PIPE,
  _______, KC_F5,   KC_F6,   KC_F7,   KC_F8,   _______, _______, KC_MINS, KC_EQL,  _______, KC_COLN, KC_DQUO,
  _______, KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, _______, _______, KC_LABK, KC_RABK, KC_QUES, LOWER,
  _______, _______, _______, KC_LBRC,     _______,      LT_HYPER(KC_BSPC),KC_RBRC, _______, _______, _______
),

[RAISE_LAYER] = LAYOUT_preonic_2x2u(
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, _______, KC_BTN2, KC_MS_U, KC_BTN1, _______, _______, CTL_PUP, KC_UP,   CTL_PDN, _______, _______,
  _______, _______, KC_MS_L, KC_MS_D, KC_MS_R, _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______,
  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, RAISE,
  _______, _______, _______, KC_LBRC, LT_HYPER(KC_DEL),     _______,      KC_RBRC, _______, _______, _______
),

[HYPER_LAYER] = LAYOUT_preonic_2x2u(
  _______, _______, _______, _______, _______, KC_MUTE, KC_MPLY, _______, AU_TOGG, MU_TOGG, REM_RGB, QK_BOOT,
  _______, KC_F13,  KC_F14,  KC_F15,  KC_F16,  KC_VOLU, KC_MNXT, _______, _______, _______, _______, _______,
  _______, KC_F17,  KC_F18,  KC_F19,  KC_F20,  KC_VOLD, KC_MPRV, _______, _______, _______, _______, _______,
  _______, KC_F21,  KC_F22,  KC_F23,  KC_F24,  _______, _______, _______, _______, _______, _______, HYPER,
  _______, _______, _______, KC_LABK,     _______,          _______,      KC_RABK, _______, HYPER,   _______
)

};