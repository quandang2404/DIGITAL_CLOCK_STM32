#ifndef FSM_H
#define FSM_H

#include <stdint.h>

/* ===== SYSTEM STATES ===== */
typedef enum {
    STATE_DISPLAY = 0,

    STATE_SET_TIME_HOUR,
    STATE_SET_TIME_MIN,

    STATE_SET_ALARM,

    STATE_SET_CD_HOUR,
    STATE_SET_CD_MIN,
    STATE_SET_CD_SEC,

    STATE_CD_RUNNING
} SystemState_t;

/* ===== GLOBAL STATE ===== */
extern volatile SystemState_t g_state;

/* ===== FSM API ===== */
void FSM_Init(void);
void FSM_Update(void);

#endif

