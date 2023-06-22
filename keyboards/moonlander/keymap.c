/* Copyright 2020 ZSA Technology Labs, Inc <@zsa>
 * Copyright 2020 Jack Humbert <jack.humb@gmail.com>
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
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

enum moonlander_layers {
  _BASE,
  _LOWER,
  _RAISE,
  _HYPER
};

#define _BASE_HSV  HSV_PURPLE
#define _LOWER_HSV HSV_BLUE
#define _RAISE_HSV HSV_GREEN
#define _HYPER_HSV HSV_RED

// Aliases for LT() to a particular layer
#define LT_BSE(kc) LT(_BASE,kc)
#define LT_LWR(kc) LT(_LOWER,kc)
#define LT_RSE(kc) LT(_RAISE,kc)
#define LT_HYP(kc) LT(_HYPER,kc)

// Aliases for other combos
#define WIN_TAB LGUI(KC_TAB)
#define VSC_MEN LCTL(LSFT(KC_P))
#define VSC_WHC MEH(KC_P)
#define MEH_SPC MEH(KC_SPC)

/*
 * Custom keycodes
 */

enum moondlander_keycodes {
  BASE = SAFE_RANGE,    // Set the default layer to _BASE
  LOWER,                // Set the default layer to _LOWER
  RAISE,                // Set the default layer to _RAISE
  HYPER,                // Set the default layer to _HYPER
  REMRGB                // Toggle remote RGB mode
};

/*
 * Keymaps
 */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_BASE] = LAYOUT_moonlander(
  KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_MUTE,           KC_MPLY, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_VOLU,           KC_MNXT, KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
  KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_VOLD,           KC_MPRV, KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                                KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
  KC_LCTL, KC_LCTL, XXXXXXX, XXXXXXX, KC_LGUI,          KC_PWR,            REMRGB,           VSC_WHC, KC_LBRC, KC_RBRC, KC_RCTL, KC_RCTL,
                               LT_LWR(KC_SPC), KC_LALT, QK_LOCK,           KC_RALT, QK_LEAD, LT_RSE(KC_ENT)
),

[_LOWER] = LAYOUT_moonlander(
  KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, _______,           _______, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_DEL,
  WIN_TAB, KC_F1,   KC_F2,   KC_F3,   KC_F4,   XXXXXXX, _______,           _______, XXXXXXX, KC_UNDS, KC_PLUS, XXXXXXX, XXXXXXX, KC_PIPE,
  MEH_SPC, KC_F5,   KC_F6,   KC_F7,   KC_F8,   XXXXXXX, _______,           _______, XXXXXXX, KC_MINS, KC_EQL,  XXXXXXX, KC_COLN, KC_DQUO,
  _______, KC_F9,   KC_F10,  KC_F11,  KC_F12,  XXXXXXX,                             XXXXXXX, XXXXXXX, KC_LABK, KC_RABK, KC_QUES, _______,
  _______, _______, XXXXXXX, XXXXXXX, _______,          LOWER,             LOWER,            VSC_MEN, KC_LCBR, KC_RCBR, _______, _______,
                                      _______, _______, _______,           _______, _______, LT_HYP(KC_BSPC)
),

[_RAISE] = LAYOUT_moonlander(
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,           _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  WIN_TAB, KC_F13,  KC_F14,  KC_F15,  KC_F16,  XXXXXXX, _______,           _______, XXXXXXX, KC_PGUP, KC_UP,   KC_PGDN, XXXXXXX, XXXXXXX,
  MEH_SPC, KC_F17,  KC_F18,  KC_F19,  KC_F20,  XXXXXXX, _______,           _______, XXXXXXX, KC_LEFT, KC_DOWN, KC_RGHT, XXXXXXX, XXXXXXX,
  _______, KC_F21,  KC_F22,  KC_F23,  KC_F24,  XXXXXXX,                             XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,
  _______, _______, XXXXXXX, XXXXXXX, _______,          RAISE,             RAISE,            VSC_MEN, XXXXXXX, XXXXXXX, _______, _______,
                              LT_HYP(KC_DEL),  _______, _______,           _______, _______, _______
),

[_HYPER] = LAYOUT_moonlander(
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, MU_TOGG,           RGB_TOG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, QK_BOOT,
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, RGB_SPI,           RGB_MOD, XXXXXXX, KC_BTN1, KC_MS_U, KC_BTN2, XXXXXXX, AU_TOGG,
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, RGB_SPD,           RGB_RMOD,XXXXXXX, KC_MS_L, KC_MS_D, KC_MS_R, XXXXXXX, XXXXXXX,
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                             XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  _______, _______, XXXXXXX, XXXXXXX, XXXXXXX,          HYPER,             HYPER,            _______, XXXXXXX, XXXXXXX, _______, _______,
                                      _______, _______, _______,           _______, _______, _______
)

};

/*
 * Songs
 */

static float remote_rgb_on_song[][2]  = SONG(NUM_LOCK_ON_SOUND);
static float remote_rgb_off_song[][2] = SONG(NUM_LOCK_OFF_SOUND);

static float sticky_on_song[][2]  = SONG(E__NOTE(_A4), E__NOTE(_A4), E__NOTE(_C5),);
static float sticky_off_song[][2] = SONG(E__NOTE(_C5), E__NOTE(_C5), E__NOTE(_A4),);

static float leader_on_song[][2] = SONG(E__NOTE(_A5), E__NOTE(_A5),);
static float leader_ok_song[][2] = SONG(E__NOTE(_A5), E__NOTE(_E6),);
static float leader_ko_song[][2] = SONG(E__NOTE(_A5), HD_NOTE(_E4),);


/*
 * Helpers
 */

uint8_t old_rgb_mode;
uint8_t old_rgb_speed;
uint8_t old_rgb_hue;
uint8_t old_rgb_sat;
uint8_t old_rgb_val;

void save_rgb_state(void) {
  old_rgb_mode = rgb_matrix_get_mode();
  old_rgb_speed = rgb_matrix_get_speed();
  old_rgb_hue = rgb_matrix_get_hue();
  old_rgb_sat = rgb_matrix_get_sat();
  old_rgb_val = rgb_matrix_get_val();
}

void restore_rgb_state(void) {
  rgb_matrix_mode_noeeprom(old_rgb_mode);
  rgb_matrix_set_speed_noeeprom(old_rgb_speed);
  rgb_matrix_sethsv_noeeprom(old_rgb_hue, old_rgb_sat, old_rgb_val);
}

/*
 * Initialization code
 */

void keyboard_post_init_user(void) {
  set_single_persistent_default_layer(_BASE);
  rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
  rgb_matrix_sethsv_noeeprom(_BASE_HSV);
}

/*
 * Leader mode
 */

#define ONE_KEY_SEQUENCE(kc1, str) \
  if (leader_sequence_one_key(kc1)) { SEND_STRING(str); return true; }
#define TWO_KEYS_SEQUENCE(kc1, kc2, str) \
  if (leader_sequence_two_keys(kc1, kc2)) { SEND_STRING(str); return true; }
#define THREE_KEYS_SEQUENCE(kc1, kc2, kc3, str) \
  if (leader_sequence_three_keys(kc1, kc2, kc3)) { SEND_STRING(str); return true; }

#define COMPOSE_KEY      SS_TAP(X_RALT)
#define LOWER_ACUTE(kc)  SS_TAP(X_QUOT) SS_TAP(kc)
#define UPPER_ACUTE(kc)  SS_TAP(X_QUOT) SS_LSFT(SS_TAP(kc))
#define LOWER_UMLAUT(kc) SS_LSFT(SS_TAP(X_QUOT)) SS_TAP(kc)
#define UPPER_UMLAUT(kc) SS_LSFT(SS_TAP(X_QUOT)) SS_LSFT(SS_TAP(kc))


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

void set_leader_mode_rgb_state(void) {
  rgb_matrix_mode_noeeprom(RGB_MATRIX_BREATHING);
  rgb_matrix_set_speed_noeeprom(255);
}

void leader_start_user(void) {
  save_rgb_state();
  set_leader_mode_rgb_state();
  PLAY_SONG(leader_on_song);
}

void leader_end_user(void) {
  bool success = process_leader_sequence();
  restore_rgb_state();
  if (success) {
    PLAY_SONG(leader_ok_song);
  } else {
    PLAY_SONG(leader_ko_song);
  }
}

/*
 * Remote RGB mode
 */

bool remote_rgb_mode = false;

#define HID_MSG_SIZE 32
uint8_t hid_buffer[HID_MSG_SIZE];

void raw_hid_receive(uint8_t *data, uint8_t length) {
  if (remote_rgb_mode) {
    rgb_matrix_sethsv_noeeprom(data[0], data[1], data[2]);
    raw_hid_send(data, length);
  }
}

void set_remote_rgb_mode_rgb_state(void) {
  rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
  rgb_matrix_sethsv_noeeprom(HSV_WHITE);
}

void toggle_remote_rgb_mode(void) {
  if(!remote_rgb_mode) {
    remote_rgb_mode = true;
    save_rgb_state();
    set_remote_rgb_mode_rgb_state();
    PLAY_SONG(remote_rgb_on_song);
  } else {
    remote_rgb_mode = false;
    restore_rgb_state();
    PLAY_SONG(remote_rgb_off_song);
  }
}

/*
 * Process custom keycodes
 */

void set_or_revert_default_layer(uint8_t layer) {
  if (IS_LAYER_ON(layer)) {
    set_single_persistent_default_layer(layer);
    PLAY_SONG(sticky_on_song);
  } else {
    set_single_persistent_default_layer(_BASE);
    layer_move(_BASE);
    PLAY_SONG(sticky_off_song);
  }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    // Sticky mode
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
    case HYPER:
      if (record->event.pressed) {
        set_or_revert_default_layer(_HYPER);
      }
      return false;
    // Remote RGB mode
    case REMRGB:
      if (record->event.pressed) {
        toggle_remote_rgb_mode();
      }
      return false;
  }

  return true;
};

/*
 * Change backlight color depending on layer
 */

layer_state_t layer_state_set_user(layer_state_t state) {

  if (remote_rgb_mode) {
    return state;
  }

  switch (get_highest_layer(state | default_layer_state)) {
    case _LOWER:
      rgb_matrix_sethsv_noeeprom(_LOWER_HSV);
      break;
    case _RAISE:
      rgb_matrix_sethsv_noeeprom(_RAISE_HSV);
      break;
    case _HYPER:
      rgb_matrix_sethsv_noeeprom(_HYPER_HSV);
      break;
    default:
      rgb_matrix_sethsv_noeeprom(_BASE_HSV);
      break;
  }
  return state;
}