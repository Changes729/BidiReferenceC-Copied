/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _OBJECT_MANAGER_H
#define _OBJECT_MANAGER_H
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
interface_data_t* object_manager_regist(dbus_object_t* data);
void              object_manager_unregist(dbus_object_t* data);

void emit_interfaces_added(struct dbus_object* data);
void emit_interfaces_removed(struct dbus_object* data);

#ifdef __cplusplus
}
#endif
#endif /* _OBJECT_MANAGER_H */
