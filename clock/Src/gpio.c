#include "gpio.h"

void GPIO_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;

    /* ========= BUZZER PA1 ========= */
    GPIOA->CRL &= ~(0xF << 4);
    GPIOA->CRL |=  (0x3 << 4);
    GPIOA->ODR &= ~(1 << 1);

    /* ========= BUTTONS ========= */
    /* PB0, PB1 input pull-up */
    GPIOB->CRL &= ~((0xF << 0) | (0xF << 4));
    GPIOB->CRL |=  ((0x8 << 0) | (0x8 << 4));

    /* PB10, PB11, PB12 input pull-up */
    GPIOB->CRH &= ~((0xF << 8) | (0xF << 12) | (0xF << 16));
    GPIOB->CRH |=  ((0x8 << 8) | (0x8 << 12) | (0x8 << 16));

    /* Enable pull-up */
    GPIOB->ODR |= (1 << 0) | (1 << 1) | (1 << 10) | (1 << 11) | (1 << 12);
}

uint8_t Button_Read(Button_t btn)
{
    switch (btn)
    {
        case BTN_MODE:       return !(GPIOB->IDR & (1 << 0));
        case BTN_COUNTDOWN:  return !(GPIOB->IDR & (1 << 1));
        case BTN_UP:         return !(GPIOB->IDR & (1 << 10));
        case BTN_DOWN:       return !(GPIOB->IDR & (1 << 11));
        case BTN_OK:         return !(GPIOB->IDR & (1 << 12));
        default: return 0;
    }
}

void Buzzer_On(void)
{
    GPIOA->BSRR = (1 << 1);
}

void Buzzer_Off(void)
{
    GPIOA->BRR = (1 << 1);
}
