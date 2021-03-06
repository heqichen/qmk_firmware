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
	// initialize row and col
    for (int y=0; y<MATRIX_ROWS; ++y) matrix[y] = 0;
    // initialize col
    palSetPadMode(GPIOA, 0, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOA, 1, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOA, 2, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOA, 3, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOA, 4, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOA, 5, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOA, 6, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(GPIOA, 7, PAL_MODE_INPUT_PULLUP);


    // initialize row
    palSetPadMode(GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(GPIOB, 0);
    palSetPadMode(GPIOB, 1, PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(GPIOB, 1);
    palSetPadMode(GPIOB, 2, PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(GPIOB, 2);
    palSetPadMode(GPIOB, 3, PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(GPIOB, 3);

    wait_ms(500);
    debug_matrix = true;
    xprintf("\r\ninitializing...\r\n");
	//wait_ms(500);

	matrix_init_quantum();
}

static int count = 0;
bool pb13 = false;
uint8_t matrix_scan(void){
    palSetPad(GPIOB, 0); {
        for (int rowIdx = 0; rowIdx < MATRIX_ROWS; rowIdx++) {
            if (rowIdx & 1) palSetPad(GPIOB, 1);
            if (rowIdx & 2) palSetPad(GPIOB, 2);
            if (rowIdx & 4) palSetPad(GPIOB, 3);
            wait_ms(1); // wait for status steable
            int port = palReadPort(GPIOA);
            matrix[rowIdx] = ~port & 0x00FF;
            palClearPad(GPIOB, 1);
            palClearPad(GPIOB, 2);
            palClearPad(GPIOB, 3);
        }
    } palClearPad(GPIOB, 0);



    count ++;
    if (count > 200) {
        count = 0;
        for (int y=0; y<MATRIX_ROWS; ++y) {
            for (int x=0; x<MATRIX_COLS; ++x) {
                xprintf("%c ", (matrix[y]&(1<<x) ? '1' : '0'));
            }
            xprintf("\r\n");
        }
        xprintf("\r\n");
    }
    matrix_scan_quantum();
    return 1;
}


inline bool matrix_is_on(uint8_t row, uint8_t col){
	return (matrix[row] & ((matrix_row_t)1<<col));
}


inline matrix_row_t matrix_get_row(uint8_t row){
	return matrix[row];
}



