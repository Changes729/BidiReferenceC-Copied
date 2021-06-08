/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _TIMER_TASK_H
#define _TIMER_TASK_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <stdint.h>

#include "typedef.h"

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef struct _timer_task timer_task_t;

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
timer_task_t* timer_task_new(uint32_t       ms,
                             operation_cb_t operation,
                             void*          user_data);
void          timer_task_del(timer_task_t*);
void          timer_task_continue(timer_task_t*);
void          timer_flush(timer_task_t*);

void     timer_run();
uint32_t timer_next_alarm();

#ifdef __cplusplus
}
#endif
#endif /* _TIMER_TASK_H */
