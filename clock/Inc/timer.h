#ifndef TIMER_H
#define TIMER_H

#include "stm32f103xb.h"

extern volatile uint32_t sysTick_ms;
void TIM2_Init(void);

#endif
