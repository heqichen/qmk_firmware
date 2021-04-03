#ifndef __DEBOUNCE_H__
#define __DEBOUNCE_H__


#include "matrix.h"

#define DEBOUNCE_THRESHOLD 3

void debounceInit(void);
void debounceTick(matrix_row_t matrixSignal[MATRIX_ROWS]);

/* whether a switch is on */
bool debouncedIsKeyOn(uint8_t row, uint8_t col);
/* matrix state on row */
matrix_row_t debouncedGetRow(uint8_t row);


#endif
