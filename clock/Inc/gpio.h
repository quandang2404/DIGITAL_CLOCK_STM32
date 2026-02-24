#ifndef GPIO_H
#define GPIO_H

#include "stm32f103xb.h"

/* Button enum */
typedef enum {
    BTN_MODE,
    BTN_COUNTDOWN,
    BTN_UP,
    BTN_DOWN,
    BTN_OK
} Button_t;

/* Init */
void GPIO_Init(void);

/* Button */
uint8_t Button_Read(Button_t btn);

/* Buzzer */
void Buzzer_On(void);
void Buzzer_Off(void);

#endif
