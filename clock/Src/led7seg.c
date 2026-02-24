#include "led7seg.h"
#define SCLK (1<<5)
#define RCLK (1<<6)
#define DIO  (1<<7)
static const uint8_t seg_map[10] =
{
  0xC0, //0
  0xF9, //1
  0xA4, //2
  0xB0, //3
  0x99, //4
  0x92, //5
  0x82, //6
  0xF8, //7
  0x80, //8
  0x90  //9
};
#define SEG_DASH  0xBF
#define SEG_BLANK 0xFF
#define SEG_DOT   0x7F
static const uint8_t digit_sel[4] = {0x01,0x02,0x04,0x08};
static uint8_t buf_m1[4];
static uint8_t buf_m2[4];

static volatile uint8_t scan = 0;
static uint8_t mode = 0;

/* ===== SHIFT OUT ===== */
static void HC595_Send(uint8_t data)
{
    for (int i = 0; i < 8; i++)
    {
        if (data & 0x80) GPIOA->BSRR = DIO;
        else             GPIOA->BRR  = DIO;

        GPIOA->BRR  = SCLK;
        GPIOA->BSRR = SCLK;

        data <<= 1;
    }
}

/* ===== INIT ===== */
void LED7SEG_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRL &= ~(0xFFF << 20);
    GPIOA->CRL |=  (0x333 << 20);
}

/* ===== MODE TEXT ===== */
void LED7SEG_SetMode(uint8_t m)
{
    mode = m;

    if (m == 0) {        // --
        buf_m1[0] = SEG_DASH;
        buf_m1[1] = SEG_DASH;
    }
    else if (m == 1) {   // CD
        buf_m1[0] = 0xC6; // C
        buf_m1[1] = 0xA1; // d
    }
    else if (m == 2) {   // SA
        buf_m1[0] = 0x92; // S
        buf_m1[1] = 0x88; // A
    }
}

/* ===== SET TIME ===== */
void LED7SEG_SetTime(uint8_t hh, uint8_t mm, uint8_t ss)
{
    /* module 1: --hh */
    buf_m1[2] = seg_map[hh / 10];
    buf_m1[3] = seg_map[hh % 10] & SEG_DOT;   // chấm giữa --.hh

    /* module 2: mm.ss */
    buf_m2[0] = seg_map[mm / 10];
    buf_m2[1] = seg_map[mm % 10] & SEG_DOT;   // chấm giữa mm.ss
    buf_m2[2] = seg_map[ss / 10];
    buf_m2[3] = seg_map[ss % 10];
}

/* ===== SCAN ISR (GỌI TRONG TIM2 IRQ) ===== */
void LED7SEG_ScanISR(void)
{
    uint8_t idx = 3 - scan;

    /* module 2 */
    HC595_Send(buf_m2[idx]);
    HC595_Send(digit_sel[scan]);

    /* module 1 */
    HC595_Send(buf_m1[idx]);
    HC595_Send(digit_sel[scan]);

    /* latch */
    GPIOA->BRR  = RCLK;
    GPIOA->BSRR = RCLK;

    scan++;
    if (scan >= 4) scan = 0;
}

