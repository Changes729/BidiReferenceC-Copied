/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DBUS_INITIAL_H
#define _DBUS_INITIAL_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <dbus/dbus-shared.h>
#include <stdbool.h>

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef struct DBusConnection DBusConnection;

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
bool dbus_init(DBusBusType      type,
               const char*      dbus_name,
               unsigned int     flag,
               DBusConnection** connection);

void dbus_final(DBusConnection* connection);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_INITIAL_H */
