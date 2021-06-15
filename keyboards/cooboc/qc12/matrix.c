/*
Copyright 2017-2019 Mathias Andersson <wraul@dbox.se>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdint.h>
#include <stdbool.h>
#include "wait.h"
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "debounce.h"
#include "quantum.h"


#if (MATRIX_COLS <= 8)
#    define print_matrix_header() print("\nr/c 01234567\n")
#    define print_matrix_row(row) print_bin_reverse8(matrix_get_row(row))
#    define matrix_bitpop(i) bitpop(qc_debounce_get_row(i))
#    define ROW_SHIFTER ((uint8_t)1)
#elif (MATRIX_COLS <= 16)
#    define print_matrix_header() print("\nr/c 0123456789ABCDEF\n")
#    define print_matrix_row(row) print_bin_reverse16(matrix_get_row(row))
#    define matrix_bitpop(i) bitpop16(qc_debounce_get_row(i))
#    define ROW_SHIFTER ((uint16_t)1)
#elif (MATRIX_COLS <= 32)
#    define print_matrix_header() print("\nr/c 0123456789ABCDEF0123456789ABCDEF\n")
#    define print_matrix_row(row) print_bin_reverse32(matrix_get_row(row))
#    define matrix_bitpop(i) bitpop32(qc_debounce_get_row(i))
#    define ROW_SHIFTER ((uint32_t)1)
#endif

static const pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;
static const pin_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;

__attribute__((weak)) void matrix_init_quantum(void) { matrix_init_kb(); }

__attribute__((weak)) void matrix_scan_quantum(void) { matrix_scan_kb(); }

__attribute__((weak)) void matrix_init_kb(void) { matrix_init_user(); }

__attribute__((weak)) void matrix_scan_kb(void) { matrix_scan_user(); }

__attribute__((weak)) void matrix_init_user(void) {}

__attribute__((weak)) void matrix_scan_user(void) {}

inline uint8_t matrix_rows(void) { return MATRIX_ROWS; }

inline uint8_t matrix_cols(void) { return MATRIX_COLS; }

inline bool matrix_is_on(uint8_t row, uint8_t col) { return (matrix_get_row(row) & ((matrix_row_t)1 << col)); }

inline matrix_row_t matrix_get_row(uint8_t row) { return qc_debounce_get_row(row); }

void matrix_print(void) {
    print_matrix_header();

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        print_hex8(row);
        print(": ");
        print_matrix_row(row);
        print("\n");
    }
}

uint8_t matrix_key_count(void) {
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += matrix_bitpop(i);
    }
    return count;
}

/* Columns
 * col / pin:    PB5  PB4  PD7  PD4
 * 0:             0    1    0    0
 * 1:             0    1    0    0
 * 2:             0    1    0    1
 * 3:             0    1    0    1
 */

/* Row pin configuration
 * row: 0    1    2    3
 * pin: PC7  PC6  PD6  PF4
 *
 */

static void unselect_rows(void) {
    for (int i=0; i<MATRIX_ROWS; ++i) {
        writePinHigh(row_pins[i]);
    }
}

static void select_row(uint8_t row) {
    unselect_rows();
    writePinLow(row_pins[row]);
}


static uint16_t start_time;
static uint8_t ledIdxMapping[12] = {0, 7, 8, 1, 6, 9, 2, 5, 10, 3, 4, 11};

void matrix_init(void) {
    start_time = timer_read();
    // initialize key pins
    for (int i=0; i<MATRIX_ROWS; ++i) {
        setPinOutput(row_pins[i]);
        writePinHigh(row_pins[i]);
    }
    for (int i=0; i<MATRIX_COLS; ++i) {
        setPinInputHigh(col_pins[i]);
    }

    qc_debounce_init();

    matrix_init_quantum();
}

static uint16_t lastPrintTime = 0;

static bool isAlreadyStartup = false;


uint8_t matrix_scan(void) {


    if (timer_read() - start_time > 120000) {
        bootloader_jump();
    }

    if (timer_read() - lastPrintTime > 500) {
        matrix_print();
        lastPrintTime = timer_read();
    }




    bool changed = false;
    for (int row=0; row<MATRIX_ROWS; ++row) {
        select_row(row);
        wait_us(30);
        for (int col=0; col<MATRIX_COLS; ++col) {
            changed |= qc_debounce_update(row, col, readPin(col_pins[col]));
        }
    }
    unselect_rows();


    if (qc_debounce_get_row(3) & (1<<1)) {
        bootloader_jump();
    }

    // show led
    uint16_t running_time = timer_read() - start_time;
    const uint16_t STAGE_PERIOD = 200;
    if (isAlreadyStartup || running_time > 3*STAGE_PERIOD) {
        // show according the key
        // rgblight_setrgb_at(0, 0, 0, 0);
        for (int r=0; r<4; ++r) {
            for (int c=0; c<3; ++c) {
                if (qc_debounce_get_row(r) & (1<<c)) {
                    rgblight_setrgb_at(50, 50, 50, ledIdxMapping[(r*3+c)]);
                    // print("keyidx: ", r*4 + c);
                } else {
                    rgblight_setrgb_at(0, 0, 0, ledIdxMapping[(r*3+c)]);
                }
            }
        }
        isAlreadyStartup = true;
    } else if (running_time > 2*STAGE_PERIOD) {
        // rgblight_setrgb_at(0, 0, 50, 0);
        rgblight_setrgb(0, 0, 50);
    } else if (running_time > STAGE_PERIOD) {
        // rgblight_setrgb_at(0, 50, 0, 0);
        rgblight_setrgb(0, 50, 0);
    } else if (running_time > 0) {
        // rgblight_setrgb_at(50, 0, 0, 0);
        rgblight_setrgb(50, 0, 0);
    }




    matrix_scan_quantum();


    return (uint8_t)changed;
}
