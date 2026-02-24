#include "stm32f103xb.h"
#include "gpio.h"
#include "timer.h"
#include "rtc.h"
#include "led7seg.h"
#define CD_TICK_PER_SEC   111   // hiệu chỉnh theo thực tế
/* ================= FSM ================= */
typedef enum {
    STATE_DISPLAY,

    /* SET TIME */
    STATE_SET_TIME_HOUR,
    STATE_SET_TIME_MIN,

    /* ALARM */
    STATE_SET_ALARM_HOUR,
    STATE_SET_ALARM_MIN,
    STATE_SET_ALARM_SEC,
    STATE_ALARM_RING,

    /* COUNTDOWN */
    STATE_SET_CD_MIN,
    STATE_SET_CD_SEC,
    STATE_CD_RUNNING,
    STATE_CD_RING

} State_t;

/* ================= BUTTON ================= */
typedef struct {
    uint8_t last;
    uint32_t time;
} Btn_t;

Btn_t btn[5];

uint8_t Button_Event(Button_t b)
{
    uint8_t cur = Button_Read(b);

    if (cur && !btn[b].last)
    {
        if (sysTick_ms - btn[b].time > 150)
        {
            btn[b].time = sysTick_ms;
            btn[b].last = 1;
            return 1;
        }
    }

    if (!cur) btn[b].last = 0;
    return 0;
}

/* ================= MAIN ================= */
int main(void)
{
    State_t state = STATE_DISPLAY;

    uint8_t hh, mm, ss;

    /* alarm */
    uint8_t alarm_h = 0, alarm_m = 0, alarm_s = 0;
    uint8_t alarm_enable = 0;

    /* countdown */
    uint32_t cd_time = 0;
    uint32_t cd_tick = 0;

    /* ring */
    uint32_t ring_time = 0;

    /* init */
    GPIO_Init();
    LED7SEG_Init();
    TIM2_Init();
    RTC_Init();

    for (int i = 0; i < 5; i++)
        btn[i].last = btn[i].time = 0;

    while (1)
    {
        RTC_GetTime(&hh, &mm, &ss);

        /* ===== MODE: ALWAYS BACK TO DISPLAY ===== */
        if (Button_Event(BTN_MODE))
        {
            Buzzer_Off();
            GPIOC->ODR |= (1 << 13);
            state = STATE_DISPLAY;
        }

        /* ===== ALARM CHECK ===== */
        if (alarm_enable &&
            hh == alarm_h &&
            mm == alarm_m &&
            ss == alarm_s &&
            state == STATE_DISPLAY)
        {
            ring_time = 0;
            state = STATE_ALARM_RING;
        }

        switch (state)
        {
        /* ================= DISPLAY ================= */
        case STATE_DISPLAY:
            LED7SEG_SetMode(0);
            LED7SEG_SetTime(hh, mm, ss);

            if (Button_Event(BTN_UP) || Button_Event(BTN_DOWN) || Button_Event(BTN_OK))
                state = STATE_SET_TIME_HOUR;

            if (Button_Event(BTN_COUNTDOWN))
            {
                alarm_h = alarm_m = alarm_s = 0;
                state = STATE_SET_ALARM_HOUR;
            }

            if (Button_Event(BTN_MODE))
            {
                cd_time = 0;
                state = STATE_SET_CD_MIN;
            }
            break;

        /* ================= SET TIME ================= */
        case STATE_SET_TIME_HOUR:
            LED7SEG_SetMode(0);
            if (Button_Event(BTN_UP))   hh = (hh + 1) % 24;
            if (Button_Event(BTN_DOWN)) hh = (hh + 23) % 24;
            if (Button_Event(BTN_OK))   state = STATE_SET_TIME_MIN;
            LED7SEG_SetTime(hh, mm, ss);
            RTC_SetTime(hh, mm, ss);
            break;

        case STATE_SET_TIME_MIN:
            if (Button_Event(BTN_UP))   mm = (mm + 1) % 60;
            if (Button_Event(BTN_DOWN)) mm = (mm + 59) % 60;

            if (Button_Event(BTN_OK))
                state = STATE_DISPLAY;

            LED7SEG_SetTime(hh, mm, ss);
            RTC_SetTime(hh, mm, ss);
            break;

        /* ================= SET ALARM ================= */
        case STATE_SET_ALARM_HOUR:
            LED7SEG_SetMode(2);
            if (Button_Event(BTN_UP))   alarm_h = (alarm_h + 1) % 24;
            if (Button_Event(BTN_DOWN)) alarm_h = (alarm_h + 23) % 24;
            if (Button_Event(BTN_OK))   state = STATE_SET_ALARM_MIN;
            LED7SEG_SetTime(alarm_h, alarm_m, alarm_s);
            break;

        case STATE_SET_ALARM_MIN:
            if (Button_Event(BTN_UP))   alarm_m = (alarm_m + 1) % 60;
            if (Button_Event(BTN_DOWN)) alarm_m = (alarm_m + 59) % 60;
            if (Button_Event(BTN_OK))   state = STATE_SET_ALARM_SEC;
            LED7SEG_SetTime(alarm_h, alarm_m, alarm_s);
            break;

        case STATE_SET_ALARM_SEC:
            if (Button_Event(BTN_UP))   alarm_s = (alarm_s + 1) % 60;
            if (Button_Event(BTN_DOWN)) alarm_s = (alarm_s + 59) % 60;

            if (Button_Event(BTN_OK))
            {
                alarm_enable = 1;
                state = STATE_DISPLAY;
            }
            LED7SEG_SetTime(alarm_h, alarm_m, alarm_s);
            break;

        case STATE_ALARM_RING:
            if (ring_time == 0) ring_time = sysTick_ms;

            Buzzer_On();
            GPIOC->ODR &= ~(1 << 13);

            LED7SEG_SetMode(2);
            LED7SEG_SetTime(hh, mm, ss);

            if (sysTick_ms - ring_time >= 3000)
            {
                Buzzer_Off();
                GPIOC->ODR |= (1 << 13);
                state = STATE_DISPLAY;
            }
            break;

        /* ================= COUNTDOWN ================= */
        case STATE_SET_CD_MIN:
            LED7SEG_SetMode(1);
            if (Button_Event(BTN_UP))   cd_time += 60;
            if (Button_Event(BTN_DOWN) && cd_time >= 60) cd_time -= 60;
            if (Button_Event(BTN_OK))   state = STATE_SET_CD_SEC;
            LED7SEG_SetTime(0, cd_time / 60, cd_time % 60);
            break;

        case STATE_SET_CD_SEC:
            if (Button_Event(BTN_UP))   cd_time++;
            if (Button_Event(BTN_DOWN) && cd_time) cd_time--;

            if (Button_Event(BTN_OK))
            {
                cd_tick = sysTick_ms;
                state = STATE_CD_RUNNING;
            }
            LED7SEG_SetTime(0, cd_time / 60, cd_time % 60);
            break;

        case STATE_CD_RUNNING:
        {
            static uint32_t acc = 0;

            LED7SEG_SetMode(1);
            LED7SEG_SetTime(0, cd_time / 60, cd_time % 60);

            acc += (sysTick_ms - cd_tick);
            cd_tick = sysTick_ms;

            while (acc >= CD_TICK_PER_SEC)
            {
                acc -= CD_TICK_PER_SEC;

                if (cd_time > 0)
                    cd_time--;
                else
                {
                    ring_time = sysTick_ms;
                    Buzzer_On();
                    GPIOC->ODR &= ~(1 << 13); // PC13 ON
                    state = STATE_CD_RING;
                    break;
                }
            }

            if (Button_Event(BTN_MODE))
            {
                cd_time = 0;
                state = STATE_DISPLAY;
            }
        }
        break;

        case STATE_CD_RING:
            if (ring_time == 0) ring_time = sysTick_ms;

            Buzzer_On();
            GPIOC->ODR &= ~(1 << 13);

            LED7SEG_SetMode(1);

            if (sysTick_ms - ring_time >= 3000)
            {
                Buzzer_Off();
                GPIOC->ODR |= (1 << 13);
                state = STATE_DISPLAY;
            }
            break;
        }
    }
}
