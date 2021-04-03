#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "ch.h"

void toneTick(void);


void tone(uint16_t freq);
void toneStop(void);
void tonePush(uint16_t freq, uint32_t dur);

#endif
