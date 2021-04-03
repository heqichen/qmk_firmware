/*
Copyright 2012 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.	If not, see <http://www.gnu.org/licenses/>.
*/

#include <ch.h>
#include <hal.h>

/*
 * scan matrix
 */
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "wait.h"
#include "buzzer.h"
#include "debounce.h"

static inline void selectRow(int rowId);
static inline void unselectAllRow(void);
static inline unsigned int readCol(void);

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];

inline uint8_t matrix_rows(void) { return MATRIX_ROWS; }
inline uint8_t matrix_cols(void) { return MATRIX_COLS; }
__attribute__ ((weak)) void matrix_init_kb(void) {}
__attribute__ ((weak)) void matrix_scan_kb(void) {}
__attribute__ ((weak)) void matrix_init_user(void) {}
__attribute__ ((weak)) void matrix_scan_user(void) {}


void matrix_print(void){
	print("\nr/c 0123456789ABCDEF\n");
	for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
	phex(row); print(": ");
	pbin_reverse16(matrix_get_row(row));
	print("\n");
	}
}

void matrix_init(void)
{
    // hardware initialization
	// initialize row and col
    for (int y=0; y<MATRIX_ROWS; ++y) matrix[y] = 0;
    // initialize col
    palSetPadMode(GPIOB, 6, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOB, 7, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOB, 8, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOB, 9, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOB, 12, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOB, 13, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOB, 14, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOB, 15, PAL_MODE_INPUT_PULLUP);


    // initialize row
    unselectAllRow();

    // debounce initiazlize

    debounceInit();

    wait_ms(500);
    debug_matrix = true;
    xprintf("\r\ninitializing...\r\n");
    toneStop();
	wait_ms(200);

	matrix_init_quantum();

}

static int count = 0;

extern uint8_t debounceCount[MATRIX_ROWS][sizeof(matrix_row_t)*8];

uint8_t matrix_scan(void){
    for (int y=0; y<7; ++y) {
        selectRow(y);
        wait_us(20);
        matrix[y] = readCol();
        unselectAllRow();
    }
    debounceTick(matrix);

    count ++;
    if (count > 150) {
        count = 0;
        for (int y=0; y<MATRIX_ROWS; ++y) {
            for (int x=0; x<MATRIX_COLS; ++x) {
                xprintf("%d ", (int)debounceCount[y][x]);
            }
            xprintf("\r\n");
        }
        xprintf("\r\n");
    }
    matrix_scan_quantum();
    return 1;
}


inline void selectRow(int rowId) {
    switch (rowId) {
        case (0): {
            palSetPadMode(GPIOB, 11, PAL_MODE_OUTPUT_PUSHPULL);
            palClearPad(GPIOB, 11);
            break;
        }
        case (1): {
            palSetPadMode(GPIOB, 10, PAL_MODE_OUTPUT_PUSHPULL);
            palClearPad(GPIOB, 10);
            break;
        }
        case (2): {
            palSetPadMode(GPIOA, 7, PAL_MODE_OUTPUT_PUSHPULL);
            palClearPad(GPIOA, 7);
            break;
        }
        case (3): {
            palSetPadMode(GPIOA, 6, PAL_MODE_OUTPUT_PUSHPULL);
            palClearPad(GPIOA, 6);
            break;
        }
        case (4): {
            palSetPadMode(GPIOA, 5, PAL_MODE_OUTPUT_PUSHPULL);
            palClearPad(GPIOA, 5);
            break;
        }
        case (5): {
            palSetPadMode(GPIOA, 4, PAL_MODE_OUTPUT_PUSHPULL);
            palClearPad(GPIOA, 4);
            break;
        }
        case (6): {
            palSetPadMode(GPIOA, 3, PAL_MODE_OUTPUT_PUSHPULL);
            palClearPad(GPIOA, 3);
            break;
        }
        case (7): {
            palSetPadMode(GPIOA, 2, PAL_MODE_OUTPUT_PUSHPULL);
            palClearPad(GPIOA, 2);
            break;
        }
        default: {
            break;
        }
    }
}

static inline void unselectAllRow(void) {
    palSetPadMode(GPIOA, 2, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOA, 2);
    palSetPadMode(GPIOA, 3, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOA, 3);
    palSetPadMode(GPIOA, 4, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOA, 4);
    palSetPadMode(GPIOA, 5, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOA, 5);
    palSetPadMode(GPIOA, 6, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOA, 6);
    palSetPadMode(GPIOA, 7, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOA, 7);
    palSetPadMode(GPIOB, 10, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOB, 10);
    palSetPadMode(GPIOB, 11, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOB, 11);
}

static int PORT_MAP[8] = {12, 13, 14, 15, 6, 7, 8, 9};

static inline unsigned int readCol(void) {
    unsigned int val = palReadPort(GPIOB);
    unsigned int ret = 0;

    for (int x=0; x<8; ++x) {
        if (val & (1<<PORT_MAP[x])) ret |= (1<<x);
    }
    return ~ret;
}


inline bool matrix_is_on(uint8_t row, uint8_t col){
    return debouncedIsKeyOn(row, col);
}


inline matrix_row_t matrix_get_row(uint8_t row){
    return debouncedGetRow(row);
}


