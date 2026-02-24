#ifndef RTC_H
#define RTC_H

#include <stdint.h>

void RTC_Init(void);
void RTC_SetTime(uint8_t hh, uint8_t mm, uint8_t ss);
void RTC_GetTime(uint8_t *hh, uint8_t *mm, uint8_t *ss);

#endif
