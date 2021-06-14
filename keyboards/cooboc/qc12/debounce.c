#include "debounce.h"
#include <stdint.h>

#define QC_DEBOUNCE_THRESHOLD 5


static uint8_t qc_debounce_count[MATRIX_ROWS][MATRIX_COLS];
static matrix_row_t matrix_status[MATRIX_ROWS];



void qc_debounce_init(void) {
    for (int row=0; row<MATRIX_ROWS; ++row) {
        for (int col=0; col<MATRIX_COLS; ++col) {
            qc_debounce_count[row][col] = 0;
        }
        matrix_status[row] = 0xFF;
    }
}

bool qc_debounce_update(uint8_t row, uint8_t col, bool value) {
    matrix_row_t oldValue = matrix_status[row] & (1 << col);
    matrix_row_t newValue = value ? (1 << col) : 0;

    if (oldValue != newValue) {
        qc_debounce_count[row][col] ++;
        if (qc_debounce_count[row][col] > QC_DEBOUNCE_THRESHOLD) {
            qc_debounce_count[row][col] = 0;
            matrix_status[row] ^= (1 << col);
            return true;
        }
    }
    return false;
}

matrix_row_t qc_debounce_get_row(uint8_t row) {
    return ~matrix_status[row];
}
