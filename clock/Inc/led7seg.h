#ifndef _LED7SEG_H_
#define _LED7SEG_H_

#include "stm32f103xb.h"

/* API */
void LED7SEG_Init(void);
void LED7SEG_SetTime(uint8_t hh, uint8_t mm, uint8_t ss);
void LED7SEG_SetMode(uint8_t mode);   // 0=-- , 1=CD , 2=SA
void LED7SEG_ScanISR(void);

#endif
