#ifndef DBUS_DEFINE_INNER_H
#define DBUS_DEFINE_INNER_H
#pragma once

#include <dbus/dbus.h>
#include <set-list.h>
#include <stdint.h>

#include "dbus_shared_def.h"

#define DBUS_INTERFACE_OBJECT_MANAGER "org.freedesktop.DBus.ObjectManager"

typedef void (*DBusDestroyFunction)(void *user_data);

typedef uint32_t                 DBusPendingPropertySet;
typedef struct DBusPropertyTable DBusPropertyTable;

typedef enum DBusSignalFlags {
  G_DBUS_SIGNAL_FLAG_DEPRECATED   = (1 << 0),
  G_DBUS_SIGNAL_FLAG_EXPERIMENTAL = (1 << 1),
} DBusSignalFlags;

typedef enum DBusMethodFlags {
  G_DBUS_METHOD_FLAG_DEPRECATED   = (1 << 0),
  G_DBUS_METHOD_FLAG_NOREPLY      = (1 << 1),
  G_DBUS_METHOD_FLAG_ASYNC        = (1 << 2),
  G_DBUS_METHOD_FLAG_EXPERIMENTAL = (1 << 3),
} DBusMethodFlags;

typedef enum DBusPropertyFlags {
  G_DBUS_PROPERTY_FLAG_DEPRECATED   = (1 << 0),
  G_DBUS_PROPERTY_FLAG_EXPERIMENTAL = (1 << 1),
} DBusPropertyFlags;

typedef bool (*DBusPropertyGetter)(const DBusPropertyTable *property,
                                   DBusMessageIter *        iter,
                                   void *                   data);

typedef void (*DBusPropertySetter)(const DBusPropertyTable *property,
                                   DBusMessageIter *        value,
                                   DBusPendingPropertySet   id,
                                   void *                   data);

typedef bool (*DBusPropertyExists)(const DBusPropertyTable *property, void *data);

typedef DBusMessage *(*DBusMethodFunction)(DBusConnection *connection,
                                           DBusMessage *   message,
                                           void *          user_data);

typedef struct DBusArgInfo
{
  const char *name;
  const char *signature;
} DBusArgInfo;

typedef struct DBusMethodTable
{
  const char *       name;
  DBusMethodFunction function;
  DBusMethodFlags    flags;
  unsigned int       privilege;
  const DBusArgInfo *in_args;
  const DBusArgInfo *out_args;
} DBusMethodTable;

typedef struct DBusSignalTable
{
  const char *       name;
  DBusSignalFlags    flags;
  const DBusArgInfo *args;
} DBusSignalTable;

struct DBusPropertyTable
{
  const char *       name;
  const char *       type;
  DBusPropertyGetter get;
  DBusPropertySetter set;
  DBusPropertyExists exists;
  DBusPropertyFlags  flags;
};

typedef struct interface_data interface_data_t;
struct interface_data
{
  char *                   name;
  const DBusMethodTable *  methods;
  const DBusSignalTable *  signals;
  const DBusPropertyTable *properties;
  sets_t                   pending_prop;
  void *                   user_data;
  DBusDestroyFunction      destroy;
};

typedef struct dbus_object dbus_object_t;
struct dbus_object
{
  DBusConnection *    conn;
  char *              path;
  char *              introspect;
  sets_t              interfaces;
  sets_t              added;
  sets_t              removed;
  sets_t              objects;
  struct dbus_object *parent;
  bool                pending_prop;
};

struct property_data
{
  DBusConnection *       conn;
  DBusPendingPropertySet id;
  DBusMessage *          message;
};

#define DBUS_ARGS(args...)                                                     \
  (const DBusArgInfo[])                                                        \
  {                                                                            \
    args, {}                                                                   \
  }

#define DBUS_METHOD(_name, _in_args, _out_args, _function)                     \
  .name = _name, .in_args = _in_args, .out_args = _out_args,                   \
  .function = _function

#define DBUS_ASYNC_METHOD(_name, _in_args, _out_args, _function)               \
  .name = _name, .in_args = _in_args, .out_args = _out_args,                   \
  .function = _function, .flags = G_DBUS_METHOD_FLAG_ASYNC

#define DBUS_DEPRECATED_METHOD(_name, _in_args, _out_args, _function)          \
  .name = _name, .in_args = _in_args, .out_args = _out_args,                   \
  .function = _function, .flags = G_DBUS_METHOD_FLAG_DEPRECATED

#define DBUS_DEPRECATED_ASYNC_METHOD(_name, _in_args, _out_args, _function)    \
  .name = _name, .in_args = _in_args, .out_args = _out_args,                   \
  .function = _function,                                                       \
  .flags    = G_DBUS_METHOD_FLAG_ASYNC | G_DBUS_METHOD_FLAG_DEPRECATED

#define DBUS_EXPERIMENTAL_METHOD(_name, _in_args, _out_args, _function)        \
  .name = _name, .in_args = _in_args, .out_args = _out_args,                   \
  .function = _function, .flags = G_DBUS_METHOD_FLAG_EXPERIMENTAL

#define DBUS_EXPERIMENTAL_ASYNC_METHOD(_name, _in_args, _out_args, _function)  \
  .name = _name, .in_args = _in_args, .out_args = _out_args,                   \
  .function = _function,                                                       \
  .flags    = G_DBUS_METHOD_FLAG_ASYNC | G_DBUS_METHOD_FLAG_EXPERIMENTAL

#define DBUS_NOREPLY_METHOD(_name, _in_args, _out_args, _function)             \
  .name = _name, .in_args = _in_args, .out_args = _out_args,                   \
  .function = _function, .flags = G_DBUS_METHOD_FLAG_NOREPLY

#define DBUS_SIGNAL(_name, _args) .name = _name, .args = _args

#define DBUS_DEPRECATED_SIGNAL(_name, _args)                                   \
  .name = _name, .args = _args, .flags = G_DBUS_SIGNAL_FLAG_DEPRECATED

#define DBUS_EXPERIMENTAL_SIGNAL(_name, _args)                                 \
  .name = _name, .args = _args, .flags = G_DBUS_SIGNAL_FLAG_EXPERIMENTAL

#endif  // !DBUS_DEFINE_INNER_H