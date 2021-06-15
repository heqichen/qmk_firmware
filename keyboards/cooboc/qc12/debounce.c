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

static int magicbits = 0;
static int magiccycle = 0;

void updateMagicKey(int kc, int status) {
    int off = -1;
    switch (kc) {
        case (0): {off = 0; break;}
        case (2): {off = 1; break;}
        case (12): {off = 2; break;}
        case (14): {off = 3; break;}
        default: {off = -1; break;}
    }
    if (off < 0) {
        magicbits = 0;
        magiccycle = 0;
    } else {
        if (!status) {
            magicbits |= (1 << off);
            if (magicbits == 0x0F) {
                if (magiccycle % 2 == 0) {
                    magiccycle ++;
                    // xprintf("magic cycle on to %d", magiccycle);
                } else {
                    magiccycle = 0;
                }
            }
        } else {
            magicbits &= ~(1 << off);
            if (magicbits == 0x00) {
                if (magiccycle %2 == 1) {
                    magiccycle ++;
                    // xprintf("magic cycle on off %d", magiccycle);
                    if (magiccycle == 6) {
                        bootloader_jump();
                    }
                } else {
                    magiccycle = 0;
                }
            }
        }
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
            updateMagicKey(row*4 + col, newValue);
            return true;
        }
    }
    return false;
}

matrix_row_t qc_debounce_get_row(uint8_t row) {
    return ~matrix_status[row];
}
