#include QMK_KEYBOARD_H



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[0] = LAYOUT( \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  RGB_VAI, RGB_HUI, KC_NO, KC_NO, \
  RGB_VAD, RGB_HUD, KC_NO, KC_NO, \
  RGB_TOG, KC_NO, KC_NO, KC_NO \
)
};
