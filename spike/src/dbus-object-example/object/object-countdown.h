/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _OBJECT_COUNTDOWN_H
#define _OBJECT_COUNTDOWN_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <dbus/dbus.h>

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
int  register_countdown_object(DBusConnection* connection);
void unregister_countdown_object(DBusConnection* connection);

#ifdef __cplusplus
}
#endif
#endif /* _OBJECT_COUNTDOWN_H */
