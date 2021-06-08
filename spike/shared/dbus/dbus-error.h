/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DBUS_ERROR_H
#define _DBUS_ERROR_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <dbus/dbus.h>

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/* Public define -------------------------------------------------------------*/
#define ERROR_INTERFACE "org.bluez.Error"

/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
DBusMessage *dbus_create_error(DBusMessage *message,
                               const char * name,
                               const char * format,
                               ...) __attribute__((format(printf, 3, 4)));

DBusMessage *dbus_create_error_valist(DBusMessage *message,
                                      const char * name,
                                      const char * format,
                                      va_list      args);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_ERROR_H */
