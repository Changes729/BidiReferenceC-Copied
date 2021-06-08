/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DBUS_XML_H
#define _DBUS_XML_H
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
dbus_object_t* attach_dbus_object(DBusConnection* conn, const char* path);
void           detach_dbus_object(DBusConnection* conn, const char* path);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_XML_H */
