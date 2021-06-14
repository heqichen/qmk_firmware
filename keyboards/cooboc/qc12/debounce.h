#ifndef __DEBOUNCE_H__
#define __DEBOUNCE_H__

#include "quantum.h"

void qc_debounce_init(void);
bool qc_debounce_update(uint8_t row, uint8_t col, bool value);
matrix_row_t qc_debounce_get_row(uint8_t row);
#endif


