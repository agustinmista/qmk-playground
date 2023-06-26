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
  REMRGB                // Toggle remote RGB mode
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
#define TASK_VW LCA(KC_TAB)
#define VSC_MEN LCTL(LSFT(KC_P))
#define VSC_WHC MEH(KC_P)
#define MEH_SPC MEH(KC_SPC)

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
  set_single_persistent_default_layer(BASE_LAYER);
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
  // leader + T ==> Ctrl+Shift+t
  ONE_KEY_SEQUENCE(KC_T, SS_LCTL(SS_LSFT(SS_TAP(X_T))));
  // leader + Q ==> Alt+F4
  ONE_KEY_SEQUENCE(KC_Q, SS_LALT(SS_TAP(X_F4)));
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
 * Process custom keycodes
 */

// Set a given layer or revert to the base one if the given layer is already set
void set_or_revert_default_layer(uint8_t layer) {
  if (IS_LAYER_ON(layer)) { // The layer is already set, move to the base layer
    set_single_persistent_default_layer(layer);
    PLAY_SONG(sticky_on_song);
  } else { // The layer is not set, move to it
    set_single_persistent_default_layer(BASE_LAYER);
    layer_move(BASE_LAYER);
    PLAY_SONG(sticky_off_song);
  }
}

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
    // Remote RGB mode
    case REMRGB:
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
  KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
  KC_LCTL, KC_LCTL, KC_LALT, KC_LGUI, LT_LOWER(KC_SPC), LT_RAISE(KC_ENT), QK_LEAD, VSC_WHC, BASE,    KC_RCTL
),

[LOWER_LAYER] = LAYOUT_preonic_2x2u(
  KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_DEL,
  MEH_SPC, KC_F1,   KC_F2,   KC_F3,   KC_F4,   XXXXXXX, XXXXXXX, KC_UNDS, KC_PLUS, XXXXXXX, XXXXXXX, KC_PIPE,
  TASK_VW, KC_F5,   KC_F6,   KC_F7,   KC_F8,   XXXXXXX, XXXXXXX, KC_MINS, KC_EQL,  XXXXXXX, KC_COLN, KC_DQUO,
  _______, KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_LCBR, KC_RCBR, XXXXXXX, KC_LABK, KC_RABK, KC_QUES, _______,
  _______, _______, _______, _______,     _______,      LT_HYPER(KC_DEL), _______, _______, LOWER,   _______
),

[RAISE_LAYER] = LAYOUT_preonic_2x2u(
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  MEH_SPC, XXXXXXX, KC_BTN1, KC_MS_U, KC_BTN2, XXXXXXX, XXXXXXX, KC_PGUP, KC_UP,   KC_PGDN, XXXXXXX, XXXXXXX,
  TASK_VW, XXXXXXX, KC_MS_L, KC_MS_D, KC_MS_R, XXXXXXX, XXXXXXX, KC_LEFT, KC_DOWN, KC_RGHT, XXXXXXX, XXXXXXX,
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_LBRC, KC_RBRC, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,
  _______, _______, _______, _______, LT_HYPER(KC_BSPC),    _______,      _______, _______, RAISE,   _______
),

[HYPER_LAYER] = LAYOUT_preonic_2x2u(
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_MUTE, KC_MPLY, KC_7,    KC_8,    KC_9,    KC_PSLS, QK_BOOT,
  _______, KC_F13,  KC_F14,  KC_F15,  KC_F16,  KC_VOLU, KC_MNXT, KC_4,    KC_5,    KC_6,    KC_PAST, AU_TOGG,
  _______, KC_F17,  KC_F18,  KC_F19,  KC_F20,  KC_VOLD, KC_MPRV, KC_1,    KC_2,    KC_3,    KC_PMNS, REMRGB,
  _______, KC_F21,  KC_F22,  KC_F23,  KC_F24,  KC_LABK, KC_RABK, KC_0,    KC_COMM, KC_DOT,  KC_PPLS, MU_TOGG,
  _______, _______, _______, _______,     _______,          _______,      _______, _______, HYPER,   _______
)

};