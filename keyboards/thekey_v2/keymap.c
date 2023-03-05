/* Copyright 2022 Jason Wihardja
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

#include "raw_hid.h"
#include "print.h"

// The keymap
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [0] = LAYOUT(LT(1, KC_F20), LT(2, KC_F21), LT(3, KC_F22)),
  [1] = LAYOUT(      _______,       KC_F23,        KC_F24),
  [2] = LAYOUT(      RGB_RMOD,      _______,       RGB_MOD),
  [3] = LAYOUT(      RGB_VAD,       RGB_VAI,       _______),

};

// Raw HID echo
void raw_hid_receive(uint8_t *data, uint8_t length) {

  uint8_t hue = data[0];
  rgblight_sethsv_noeeprom(hue, 255, rgblight_get_val());
  raw_hid_send(data, length);
}

// Runs just one time when the keyboard initializes
void matrix_init_user(void) {
  rgblight_mode(RGBLIGHT_MODE_STATIC_LIGHT);
}

#define HID_MSG_SIZE 32
uint8_t hid_buffer[HID_MSG_SIZE];

// Change the color depending on layer
layer_state_t layer_state_set_user(layer_state_t state) {
  switch (get_highest_layer(state)) {
  case 1:
    rgblight_sethsv_noeeprom(63, 255, rgblight_get_val());
    hid_buffer[0] = 63;
    raw_hid_send(hid_buffer, HID_MSG_SIZE);
    break;
  case 2:
    rgblight_sethsv_noeeprom(127, 255, rgblight_get_val());
    hid_buffer[0] = 127;
    raw_hid_send(hid_buffer, HID_MSG_SIZE);
    break;
  case 3:
    rgblight_sethsv_noeeprom(191, 255, rgblight_get_val());
    hid_buffer[0] = 191;
    raw_hid_send(hid_buffer, HID_MSG_SIZE);
    break;
  default:
    rgblight_sethsv_noeeprom(0, 255, rgblight_get_val());
    hid_buffer[0] = 0;
    raw_hid_send(hid_buffer, HID_MSG_SIZE);
    break;
  }
  return state;
}