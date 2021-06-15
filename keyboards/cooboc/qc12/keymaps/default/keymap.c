#include QMK_KEYBOARD_H



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[0] = LAYOUT( \
  KC_NO, KC_NO, KC_NO, KC_NO, \
  RGB_VAI, RGB_HUI, KC_NO, KC_NO, \
  RGB_VAD, RGB_HUD, KC_NO, KC_NO, \
  RGB_TOG, KC_NO, KC_NO, KC_NO \
)
};


bool encoder_update_user(uint8_t index, bool clockwise) {
    switch (index) {
        case (0): {
            tap_code(clockwise ? KC_A: KC_B);
            break;
        }
        case (1): {
            tap_code(clockwise ? KC_C: KC_D);
            break;
        }
        case (2): {
            tap_code(clockwise ? KC_E: KC_F);
            break;
        }
        default: {
            break;
        }
    }
    return false;
}
