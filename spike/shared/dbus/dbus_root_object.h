/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _ROOT_OBJECT_H
#define _ROOT_OBJECT_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include "dbus_shared_def.h"

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
int  register_root_object(DBusConnection* connection);
void unregister_root_object(DBusConnection* connection);

#ifdef __cplusplus
}
#endif
#endif /* _ROOT_OBJECT_H */
