#include "buzzer.h"

#include <hal.h>
#include "ch.h"

static PWMConfig pwmcfg = {
  4000, // frequency                      /* 10kHz PWM clock frequency.     */
  10,                                     /* Initial PWM period 1S.         */
  NULL,                                     /* Period callback.               */
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},          /* CH1 mode and callback.         */
   {PWM_OUTPUT_DISABLED, NULL},             /* CH2 mode and callback.         */
   {PWM_OUTPUT_DISABLED, NULL},             /* CH3 mode and callback.         */
   {PWM_OUTPUT_DISABLED, NULL}              /* CH4 mode and callback.         */
  },
  0,                                        /* Control Register 2.            */
  0
};



void tone(uint16_t freq) {
    if (freq > 0) {
        pwmcfg.frequency = (uint32_t)freq * 10;
        palSetPadMode(GPIOA, 8, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
        pwmStart(&PWMD1, &pwmcfg);
        pwmEnableChannel(&PWMD1, 0, 5);
    } else {
        toneStop();
    }

}
void toneStop(void) {
    palSetPadMode(GPIOA, 8, PAL_MODE_OUTPUT_PUSHPULL);
    palClearPad(GPIOA, 8);
}

struct ToneNote {
    uint16_t frequency;
    uint32_t duration;
};

#define QUEUE_SIZE 100

static struct ToneNote noteQueue[QUEUE_SIZE];
static int noteSize = 0;
static bool isPlaying = false;
static systime_t noteStartTime = 0U;



void toneTick(void) {
    systime_t now = chVTGetSystemTime();
    if (isPlaying) {
        // check if stop
        if ((now - noteStartTime) > noteQueue[0].duration) { // stop
            for (size_t i=0; i<noteSize-1; ++i) {
                noteQueue[i] = noteQueue[i+1];
            }
            isPlaying = false;
            noteSize--;
            toneStop();
        }
    } else {
        if (noteSize > 0) {
            noteStartTime = now;
            isPlaying = true;
            tone(noteQueue[0].frequency);
        }
    }
}

void tonePush(uint16_t freq, uint32_t dur) {
    if (noteSize < QUEUE_SIZE) {
        noteQueue[noteSize].frequency = freq;
        noteQueue[noteSize].duration = dur;
        noteSize ++;
    }
}





