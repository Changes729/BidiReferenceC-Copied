#ifndef DBUS_DEFINE_SHARED_H
#define DBUS_DEFINE_SHARED_H

#include <stdbool.h>

typedef struct DBusConnection  DBusConnection;
typedef struct DBusMessageIter DBusMessageIter;
typedef struct DBusMessage     DBusMessage;

typedef enum DBusPropertyChangedFlags {
  G_DBUS_PROPERTY_CHANGED_FLAG_FLUSH = (1 << 0),
} DBusPropertyChangedFlags;

#endif  // !DBUS_DEFINE_SHARED_H