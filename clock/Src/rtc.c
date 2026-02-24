#include "stm32f103xb.h"
#include "rtc.h"

/* ===== LOCAL ===== */

static void RTC_EnterConfig(void)
{
    RTC->CRL |= RTC_CRL_CNF;
}

static void RTC_ExitConfig(void)
{
    RTC->CRL &= ~RTC_CRL_CNF;
    while (!(RTC->CRL & RTC_CRL_RTOFF));
}

static void RTC_WaitForSync(void)
{
    RTC->CRL &= ~RTC_CRL_RSF;
    while (!(RTC->CRL & RTC_CRL_RSF));
}

static uint32_t RTC_ReadCounter(void)
{
    uint32_t h1, l, h2;
    do {
        h1 = RTC->CNTH;
        l  = RTC->CNTL;
        h2 = RTC->CNTH;
    } while (h1 != h2);

    return (h1 << 16) | l;
}

/* ===== PUBLIC ===== */

void RTC_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
    PWR->CR |= PWR_CR_DBP;

    if (!(RCC->BDCR & RCC_BDCR_RTCEN))
    {
        RCC->BDCR |= RCC_BDCR_LSEON;
        while (!(RCC->BDCR & RCC_BDCR_LSERDY));

        RCC->BDCR &= ~RCC_BDCR_RTCSEL;
        RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;
        RCC->BDCR |= RCC_BDCR_RTCEN;

        RTC_EnterConfig();
        RTC->PRLH = 0;
        RTC->PRLL = 32768 - 1;
        RTC_ExitConfig();
    }

    RTC_WaitForSync();
}

void RTC_SetTime(uint8_t hh, uint8_t mm, uint8_t ss)
{
    uint32_t cnt = hh * 3600UL + mm * 60UL + ss;

    RTC_EnterConfig();
    RTC->CNTH = (cnt >> 16) & 0xFFFF;
    RTC->CNTL = cnt & 0xFFFF;
    RTC_ExitConfig();
}

void RTC_GetTime(uint8_t *hh, uint8_t *mm, uint8_t *ss)
{
    uint32_t cnt;

    RTC_WaitForSync();
    cnt = RTC_ReadCounter();

    *ss = cnt % 60;
    cnt /= 60;
    *mm = cnt % 60;
    *hh = (cnt / 60) % 24;
}
