/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PROPERTIES_H
#define _PROPERTIES_H
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
interface_data_t* properties_regist(dbus_object_t* data);
void              properties_unregist(dbus_object_t* data);

void append_properties(struct interface_data* data, DBusMessageIter* iter);

void emit_property_changed_full(DBusConnection*          connection,
                                const char*              path,
                                const char*              interface,
                                const char*              name,
                                DBusPropertyChangedFlags flags);

void process_property_changes(dbus_object_t* data);
void process_properties_from_interface(dbus_object_t*         data,
                                       struct interface_data* iface);

#define emit_property_changed(conn, path, interface, name)                     \
  emit_property_changed_full(conn, path, interface, name, 0)

#ifdef __cplusplus
}
#endif
#endif /* _PROPERTIES_H */
