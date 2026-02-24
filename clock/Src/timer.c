#include "timer.h"
#include "led7seg.h"

volatile uint32_t sysTick_ms = 0;

void TIM2_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = 7200 - 1;
    TIM2->ARR = 10 - 1;

    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->CR1  |= TIM_CR1_CEN;

    NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;

        sysTick_ms++;
        LED7SEG_ScanISR();
    }
}
