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
  HYPER_LAYER,
  GAME_LAYER
};

/*
 * Layers colors
 */

#define BASE_RGB RGB_BLACK
#define LOWER_RGB RGB_BLUE
#define RAISE_RGB RGB_GREEN
#define HYPER_RGB RGB_RED
#define GAME_RGB RGB_PURPLE
#define LEADER_RGB RGB_CYAN

/*
 * Custom keycodes
 */

enum keyboard_keycodes {
  BASE = SAFE_RANGE,    // Set the default layer to BASE_LAYER
  LOWER,                // Set the default layer to LOWER_LAYER
  RAISE,                // Set the default layer to RAISE_LAYER
  HYPER,                // Set the default layer to HYPER_LAYER
  GAME,                 // Set the default later to GAME_LAYER
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
#define HR_A    LCTL_T(KC_A)
#define HR_S    LSFT_T(KC_S)
#define HR_D    LALT_T(KC_D)
#define HR_F    LGUI_T(KC_F)

#define HR_J    LGUI_T(KC_J)
#define HR_K    LALT_T(KC_K)
#define HR_L    LSFT_T(KC_L)
#define HR_SCLN LCTL_T(KC_SCLN)

// Aliases for other combos
#define CTL_PUP LCTL(KC_PGUP)
#define CTL_PDN LCTL(KC_PGDN)

// Aliases for MEH
#define MEH_TAB MEH(KC_TAB)

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
  rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
  rgb_matrix_set_color_all(BASE_RGB);
}

/*
 * RGB helpers
 */

// Change a single key in the RGB matrix to the color of a given layer
void rgb_matrix_set_color_by_layer(uint8_t index, uint8_t layer) {
  switch(layer) {
    case BASE_LAYER:
      rgb_matrix_set_color(index, BASE_RGB);
      break;
    case LOWER_LAYER:
      rgb_matrix_set_color(index, LOWER_RGB);
      break;
    case RAISE_LAYER:
      rgb_matrix_set_color(index, RAISE_RGB);
      break;
    case HYPER_LAYER:
      rgb_matrix_set_color(index, HYPER_RGB);
      break;
    case GAME_LAYER:
      rgb_matrix_set_color(index, GAME_RGB);
      break;
  }
}

/*
 * Remote RGB mode
 */

bool remote_rgb_mode = false;

typedef enum {
  REMOTE_RGB_START = 0,
  REMOTE_RGB_STOP,
  REMOTE_RGB_SET_COLOR
} REMOTE_RGB_MESSAGE_KIND;

// Toggle the remote RGB mode on and off
void remote_rgb_start(void) {
  rgb_matrix_set_color_all(RGB_BLACK);
  PLAY_SONG(remote_rgb_on_song);
  remote_rgb_mode = true;
}

void remote_rgb_stop(void) {
  PLAY_SONG(remote_rgb_off_song);
  remote_rgb_mode = false;
}

void remote_rgb_toggle(void) {
  if(!remote_rgb_mode) {
    remote_rgb_start();
  } else {
    remote_rgb_stop();
  }
}

// Parse a SET_COLOR message and set the RGB matrix accordingly:
// * data[0]: message_kind
// * data[1-3]: r,g,b values
// * data[4]: count
// * data[5-7]: unused
// * data[8-31]: payload (up to 12 sequential pairs of row/column indices)
void remote_rgb_set_color(uint8_t *data) {
  uint8_t r = data[1], g = data[2], b = data[3];
  uint8_t count = data[4];
  uint8_t *payload = &data[8];
  for (int i = 0; i < count; i++) {
    uint8_t row = payload[2*i], col = payload[2*i+1];
    uint8_t index = g_led_config.matrix_co[row][col];
    rgb_matrix_set_color(index, r, g, b);
  }
}

// Dispatch incoming HID messages
void raw_hid_receive(uint8_t *data, uint8_t length) {
  switch (data[0]) {
    case REMOTE_RGB_START:
      remote_rgb_start();
      break;
    case REMOTE_RGB_STOP:
      remote_rgb_stop();
      break;
    case REMOTE_RGB_SET_COLOR:
      if (remote_rgb_mode) {
        remote_rgb_set_color(data);
      }
      break;
    default:
      break;
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
  if (!remote_rgb_mode) {
    rgb_matrix_set_color_all(LEADER_RGB);
  }
  PLAY_SONG(leader_on_song);
  leader_mode = true;
}

// End leader mode hook
void leader_end_user(void) {
  bool success = process_leader_sequence();
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
    case GAME:
      if (record->event.pressed) {
        set_or_revert_default_layer(GAME_LAYER);
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
 * Change specific key colors depending on layer
 */

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {

  // Do not repaint if we are in a mode that overrides the default layer color
  if (leader_mode || remote_rgb_mode) {
    return false;
  }

  // Get the current layer
  uint8_t layer = get_highest_layer(layer_state | default_layer_state);

  // Paint each key
  for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
    for (uint8_t col = 0; col < MATRIX_COLS; ++col) {

      // Extract the index and keycode of the key
      uint8_t index = g_led_config.matrix_co[row][col];
      uint16_t keycode = keymap_key_to_keycode(layer, (keypos_t){col, row});

      // Only paint keys that are within bounds and have LEDs
      if (index >= led_min && index < led_max && index != NO_LED) {

        // Paint only keys that have something mapped to them using the current
        // layer color or the base layer color if they are transparent.
        switch(keycode) {
          case KC_NO:
            rgb_matrix_set_color(index, RGB_BLACK);
            break;
          case KC_TRNS:
            rgb_matrix_set_color_by_layer(index, BASE_LAYER);
            break;
          default:
            rgb_matrix_set_color_by_layer(index, layer);
            break;
        }
      }
    }
  }

  return false;
}

/*
 *  Music mode keymap
 */

const uint8_t music_map[MATRIX_ROWS][MATRIX_COLS] = {
  // Left half
  {  0,   0,   0,   0,   0,   0,   0  },
  { 25,  26,  27,  28,  29,  30,   0  },
  { 13,  14,  15,  16,  17,  18,   0  },
  {  1,   2,   3,   4,   5,   6,   0  },
  {  0,   0,   0,   0,   0,   0,   0  },
  {  0,   0,   0,   0,   0,   0,   0  },

  // Right half
  {  0,   0,   0,   0,   0,   0,   0  },
  {  0,  31,  32,  33,  34,  35,  36  },
  {  0,  19,  20,  21,  22,  23,  24  },
  {  0,   7,   8,   9,  10,  11,  12  },
  {  0,   0,   0,   0,   0,   0,   0  },
  {  0,   0,   0,   0,   0,   0,   0  }
};

/*
 * Keymaps
 */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[BASE_LAYER] = LAYOUT_moonlander(
  KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_MUTE,           KC_MPLY, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_VOLU,           KC_MNXT, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
  KC_ESC,  HR_A,    HR_S,    HR_D,    HR_F,    KC_G,    KC_VOLD,           KC_MPRV, KC_H,    HR_J,    HR_K,    HR_L,    HR_SCLN, KC_QUOT,
  KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                                KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
  MEH_F5,  MEH_F6,  MEH_F7,  MEH_F8,  KC_LCBR,          QK_LEAD,           QK_LEAD,          KC_RCBR, MEH_F9,  MEH_F10, MEH_F11, MEH_F12,
                             LT_LOWER(KC_SPC), MEH_F1,  MEH_F2,            MEH_F3,  MEH_F4,  LT_RAISE(KC_ENT)
  ),

[LOWER_LAYER] = LAYOUT_moonlander(
  KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, _______,           _______, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_DEL,
  MEH_TAB, KC_F1,   KC_F2,   KC_F3,   KC_F4,   _______, _______,           _______, _______, KC_UNDS, KC_PLUS, _______, _______, KC_PIPE,
  _______, KC_F5,   KC_F6,   KC_F7,   KC_F8,   _______, _______,           _______, _______, KC_MINS, KC_EQL,  _______, KC_COLN, KC_DQUO,
  _______, KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______,                             _______, _______, KC_LABK, KC_RABK, KC_QUES, _______,
  _______, _______, _______, _______, KC_LBRC,          LOWER,             LOWER,            KC_RBRC, _______, _______, _______, _______,
                                      _______, _______, _______,           _______, _______, LT_HYPER(KC_BSPC)
),

[RAISE_LAYER] = LAYOUT_moonlander(
  _______, _______, _______, _______, _______, _______, _______,           _______, _______, _______, _______, _______, _______, _______,
  MEH_TAB, _______, KC_BTN2, KC_MS_U, KC_BTN1, _______, _______,           _______, _______, CTL_PUP, KC_UP,   CTL_PDN, _______, _______,
  _______, _______, KC_MS_L, KC_MS_D, KC_MS_R, _______, _______,           _______, _______, KC_LEFT, KC_DOWN, KC_RGHT, _______, _______,
  _______, _______, _______, _______, _______, _______,                             _______, _______, _______, _______, _______, _______,
  _______, _______, _______, _______, KC_LBRC,          RAISE,             RAISE,            KC_RBRC, _______, _______, _______, _______,
                             LT_HYPER(KC_DEL), _______, _______,           _______, _______, _______
),

[HYPER_LAYER] = LAYOUT_moonlander(
  _______, _______, _______, _______, _______, _______, _______,           GAME,    _______, KC_7,    KC_8,    KC_9,    _______, QK_BOOT,
  _______, KC_F13,  KC_F14,  KC_F15,  KC_F16,  _______, _______,           _______, _______, KC_4,    KC_5,    KC_6,    _______, REM_RGB,
  _______, KC_F17,  KC_F18,  KC_F19,  KC_F20,  _______, _______,           _______, _______, KC_1,    KC_2,    KC_3,    _______, MU_TOGG,
  _______, KC_F21,  KC_F22,  KC_F23,  KC_F24,  _______,                             _______, KC_0,    KC_COMM, KC_DOT, _______, AU_TOGG,
  _______, _______, _______, _______, KC_LABK,          HYPER,             HYPER,            KC_RABK, _______, _______, _______, _______,
                                      _______, _______, _______,           _______, _______, _______
),

[GAME_LAYER] = LAYOUT_moonlander(
  KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_MUTE,           KC_MPLY, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_VOLU,           KC_MNXT, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
  KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_VOLD,           KC_MPRV, KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                                KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
  KC_LCTL, XXXXXXX, XXXXXXX, XXXXXXX, KC_LCBR,          QK_LEAD,           QK_LEAD,          KC_RCBR, XXXXXXX, XXXXXXX, XXXXXXX, KC_RCTL,
                             LT_LOWER(KC_SPC), KC_LALT, GAME,              GAME,    KC_LGUI, LT_RAISE(KC_ENT)
)};
