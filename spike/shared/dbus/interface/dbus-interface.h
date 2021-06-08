/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _DBUS_INTERFACE_HOME_H
#define _DBUS_INTERFACE_HOME_H
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
bool find_interface_method(struct dbus_object* dbus_object,
                           const char*         iface_name,
                           const char*         method_name,
                           DBusMethodFunction* method);

interface_data_t* add_interface(dbus_object_t*           data,
                                const char*              name,
                                const DBusMethodTable*   methods,
                                const DBusSignalTable*   signals,
                                const DBusPropertyTable* properties,
                                void*                    user_data,
                                DBusDestroyFunction      destroy);

void remove_interface(dbus_object_t* data, const char* name);
bool find_interface_by_name(struct dbus_object*     dbus_object,
                            const char*             name,
                            struct interface_data** interface);

#ifdef __cplusplus
}
#endif
#endif /* _DBUS_INTERFACE_HOME_H */
