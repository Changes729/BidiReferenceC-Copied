/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DBUS_TASK_H
#define _DBUS_TASK_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include "dbus-inner-def.h"

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
void queue_dispatch(DBusConnection *connection);
void queue_pading(dbus_object_t *data);

void message_dispatch(void *data);
void process_pading(void *user_data);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_TASK_H */
