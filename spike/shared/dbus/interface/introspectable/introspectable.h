/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _INTROSPECTABLE_H
#define _INTROSPECTABLE_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include "dbus-inner-def.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
interface_data_t* introspectable_regist(dbus_object_t* data);
void              introspectable_unregist(dbus_object_t* data);

#ifdef __cplusplus
}
#endif
#endif /* _INTROSPECTABLE_H */
