#include "debounce.h"
#include "ch.h"
#include "buzzer.h"
#include "print.h"

static matrix_row_t lastStatus[MATRIX_ROWS];
uint8_t debounceCount[MATRIX_ROWS][sizeof(matrix_row_t)*8];

static void keyFlapped(uint8_t row, uint8_t col);
static void printKeyStatus(uint8_t keyid, bool status);
static void freqModulation(uint8_t keyid, bool status);
static void morseModulation(uint8_t keyid, bool status);
static void pushTone(const char *code);

void debounceInit(void) {
    for (size_t y=0; y<MATRIX_ROWS; ++y) {
        lastStatus[y] = 0;
        for (size_t x=0; x<sizeof(matrix_row_t)*8; ++x) {
            debounceCount[y][x] = 0;
        }
    }
}

void debounceTick(matrix_row_t matrixSignal[MATRIX_ROWS]) {
    for (size_t row=0; row<MATRIX_ROWS; ++row) {
        // lastStatus[row] = matrixSignal[row];
        // continue;
        matrix_row_t rowDiff = lastStatus[row] ^ matrixSignal[row];
        for (size_t col=0; col<sizeof(matrix_row_t)*8; ++col) {
            if ((rowDiff & ((matrix_row_t)1 << col)) != 0) { // diff
                debounceCount[row][col] ++;
                if (debounceCount[row][col] > DEBOUNCE_THRESHOLD) {
                    debounceCount[row][col] = 0;
                    lastStatus[row] ^= ((matrix_row_t)1 << col); // flap the status
                    keyFlapped(row, col);
                }
            } else {
                debounceCount[row][col] = 0;
            }
        }
    }
    toneTick();
}


inline bool debouncedIsKeyOn(uint8_t row, uint8_t col) {
    return (lastStatus[row] & ((matrix_row_t)1<<col));
}


inline matrix_row_t debouncedGetRow(uint8_t row) {
    return lastStatus[row];
}

static inline void keyFlapped(uint8_t row, uint8_t col) {
    uint8_t keyid = ((uint8_t)(row)*8*sizeof(matrix_row_t) + col);
    bool status = (lastStatus[row] & ((matrix_row_t)1 << col)) != 0;

    printKeyStatus(keyid, status);
    // freqModulation(keyid, status);
    morseModulation(keyid, status);
}

static inline void printKeyStatus(uint8_t keyid, bool status) {
    xprintf("%d %s\r\n", (int)(keyid), status?"pressed":"released");
}


static inline void freqModulation(uint8_t keyid, bool status) {

    if (status) {
        tone(200 + (int)keyid * 10);
    } else {
        toneStop();
    }
}

// debouncedIsKeyOn(4, 3) || debouncedIsKeyOn(4, 6)
// debouncedIsKeyOn(5, 3) || debouncedIsKeyOn(5, 6)

static inline void morseModulation(uint8_t keyid, bool status) {
    if (!status) return ;
    if (debouncedIsKeyOn(4, 3) || debouncedIsKeyOn(4, 6)) {
        switch (keyid) {
            case (17): {pushTone(".----"); break; } // 1
            case (25): {pushTone("..---"); break; } // 2
            case (33): {pushTone("...--"); break; } // 3
            case (41): {pushTone("....-"); break; } // 4
            case (49): {pushTone("....."); break; } // 5
            case (53): {pushTone("-...."); break; } // 6
            case (45): {pushTone("--..."); break; } // 7
            case (37): {pushTone("---.."); break; } // 8
            case (29): {pushTone("----."); break; } // 9
            case (21): {pushTone("-----"); break; } // 0
            default: {break;}
        }
        return;
    }
    switch (keyid) {
        case (17): { pushTone(".-"); break; }       // A
        case (50): { pushTone("-..."); break; }     // B
        case (34): { pushTone("-.-."); break; }     // C
        case (33): { pushTone("-.."); break; }      // D
        case (32): { pushTone("."); break; }        // E
        case (41): { pushTone("..-."); break; }     // F
        case (49): { pushTone("--."); break; }      // G
        case (53): { pushTone("...."); break; }     // H
        case (36): { pushTone(".."); break; }       // I
        case (45): { pushTone(".---"); break; }     // J
        case (37): { pushTone("-.-"); break; }      // K
        case (29): { pushTone(".-.."); break; }     // L
        case (47): { pushTone("--"); break; }       // M
        case (55): { pushTone("-."); break; }       // N
        case (28): { pushTone("---"); break; }      // O
        case (20): { pushTone(".--."); break; }     // P
        case (16): { pushTone("--.-"); break; }     // Q
        case (40): { pushTone(".-."); break; }      // R
        case (25): { pushTone("..."); break; }      // S
        case (48): { pushTone("-"); break; }        // T
        case (44): { pushTone("..-"); break; }      // U
        case (42): { pushTone("...-"); break; }     // V
        case (24): { pushTone(".--"); break; }      // W
        case (26): { pushTone("-..-"); break; }     // X
        case (52): { pushTone("-.--"); break; }     // Y
        case (18): { pushTone("--.."); break; }     // Z
        case (43): { pushTone(""); break; }     // period
        case (46): { pushTone(""); break; }     // period
        default: {break;}
    }
}

#define TEMPO_DURATION 65
static void pushTone(const char *code) {
    const char *ps = code;
    while (*ps) {
        if (*ps == '.') tonePush(300, TEMPO_DURATION);
        else tonePush(300, TEMPO_DURATION*4);
        tonePush(0, TEMPO_DURATION*2);
        ps ++;
    }
    tonePush(0, TEMPO_DURATION*2);
}




