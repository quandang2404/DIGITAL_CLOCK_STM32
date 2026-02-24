#ifndef APP_H
#define APP_H

#include <stdint.h>

/* Time */
extern volatile uint8_t hour;
extern volatile uint8_t min;
extern volatile uint8_t sec;

/* Alarm */
extern volatile uint8_t alarm_h;
extern volatile uint8_t alarm_m;
extern volatile uint8_t alarm_s;

/* Countdown */
extern volatile uint8_t cd_h;
extern volatile uint8_t cd_m;
extern volatile uint8_t cd_s;

/* App API */
void App_Init(void);
void App_1Hz_Task(void);   // gọi từ timer 1Hz

#endif

