/** See a brief introduction (right-hand button) */
#include "properties.h"
/* Private include -----------------------------------------------------------*/
#include <dbus/dbus.h>
#include <string.h>

#include "dbus-interface.h"
#include "dbus-error.h"
#include "dbus-task.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static DBusMessage *properties_get(DBusConnection *connection,
                                   DBusMessage *   message,
                                   void *          user_data);

static DBusMessage *properties_get_all(DBusConnection *connection,
                                       DBusMessage *   message,
                                       void *          user_data);

static DBusMessage *properties_set(DBusConnection *connection,
                                   DBusMessage *   message,
                                   void *          user_data);

static bool find_property(const DBusPropertyTable * array,
                          const char *              name,
                          const DBusPropertyTable **out);

static void append_property(struct interface_data *  iface,
                            const DBusPropertyTable *p,
                            DBusMessageIter *        dict);
/* Private variables ---------------------------------------------------------*/
static DBusMethodTable properties_methods[] = {
    {DBUS_METHOD("Get",
                 DBUS_ARGS({"interface", "s"}, {"name", "s"}),
                 DBUS_ARGS({"value", "v"}),
                 properties_get)},
    {DBUS_METHOD("Set",
                 DBUS_ARGS({"interface", "s"}, {"name", "s"}, {"value", "v"}),
                 NULL,
                 properties_set)},
    {DBUS_METHOD("GetAll",
                 DBUS_ARGS({"interface", "s"}),
                 DBUS_ARGS({"properties", "a{sv}"}),
                 properties_get_all)},
    {},
};

static const DBusSignalTable properties_signals[] = {
    {DBUS_SIGNAL("PropertiesChanged",
                 DBUS_ARGS({"interface", "s"},
                           {"changed_properties", "a{sv}"},
                           {"invalidated_properties", "as"}))},
    {},
};

/* Private function ----------------------------------------------------------*/
interface_data_t *properties_regist(dbus_object_t *data)
{
  return add_interface(data,
                       DBUS_INTERFACE_PROPERTIES,
                       properties_methods,
                       properties_signals,
                       NULL,
                       data,
                       NULL);
}

void properties_unregist(dbus_object_t *data)
{
  remove_interface(data, DBUS_INTERFACE_PROPERTIES);
}

void append_property(struct interface_data *  iface,
                     const DBusPropertyTable *p,
                     DBusMessageIter *        dict)
{
  DBusMessageIter entry, value;

  dbus_message_iter_open_container(dict, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
  dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &p->name);
  dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, p->type, &value);

  p->get(p, &value, iface->user_data);

  dbus_message_iter_close_container(&entry, &value);
  dbus_message_iter_close_container(dict, &entry);
}

void append_properties(struct interface_data *data, DBusMessageIter *iter)
{
  DBusMessageIter          dict;
  const DBusPropertyTable *p;

  // clang-format off
  dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
          DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
          DBUS_TYPE_STRING_AS_STRING
          DBUS_TYPE_VARIANT_AS_STRING
          DBUS_DICT_ENTRY_END_CHAR_AS_STRING,
          &dict);
  // clang-format on

  for(p = data->properties; p && p->name; p++) {
    if(p->get == NULL) continue;

    if(p->exists != NULL && !p->exists(p, data->user_data)) continue;

    append_property(data, p, &dict);
  }

  dbus_message_iter_close_container(iter, &dict);
}

void emit_property_changed_full(DBusConnection *         connection,
                                const char *             path,
                                const char *             interface,
                                const char *             name,
                                DBusPropertyChangedFlags flags)
{
  const DBusPropertyTable *property;
  dbus_object_t *          data;
  struct interface_data *  iface;

  if(path == NULL) return;

  if(!dbus_connection_get_object_path_data(connection, path, (void **)&data) ||
     data == NULL)
    return;

  if(!find_interface_by_name(data, interface, (void *)&iface)) return;

  /*
   * If ObjectManager is attached, don't emit property changed if
   * interface is not yet published
   */
  if(sets_find(&data->added, iface, NULL)) return;

  if(!find_property(iface->properties, name, &property)) return;

  if(sets_find(&iface->pending_prop, (void *)property, NULL)) return;

  data->pending_prop = TRUE;
  sets_add(&iface->pending_prop, (void *)property);

  if(flags & G_DBUS_PROPERTY_CHANGED_FLAG_FLUSH)
    process_property_changes(data);
  else
    queue_pading(data);
}

static DBusMessage *properties_get(DBusConnection *connection,
                                   DBusMessage *   message,
                                   void *          user_data)
{
  dbus_object_t *          data = user_data;
  struct interface_data *  iface;
  const DBusPropertyTable *property;
  const char *             interface, *name;
  DBusMessageIter          iter, value;
  DBusMessage *            reply;

  if(!dbus_message_get_args(message,
                            NULL,
                            DBUS_TYPE_STRING,
                            &interface,
                            DBUS_TYPE_STRING,
                            &name,
                            DBUS_TYPE_INVALID))
    return NULL;

  if(!find_interface_by_name(data, interface, (void *)&iface)) {
    return dbus_create_error(message,
                             DBUS_ERROR_INVALID_ARGS,
                             "No such interface '%s'",
                             interface);
  }

  if(!find_property(iface->properties, name, &property))
    return dbus_create_error(message,
                             DBUS_ERROR_INVALID_ARGS,
                             "No such property '%s'",
                             name);

  if(property->exists != NULL && !property->exists(property, iface->user_data))
    return dbus_create_error(message,
                             DBUS_ERROR_INVALID_ARGS,
                             "No such property '%s'",
                             name);

  if(property->get == NULL)
    return dbus_create_error(message,
                             DBUS_ERROR_INVALID_ARGS,
                             "Property '%s' is not readable",
                             name);

  reply = dbus_message_new_method_return(message);
  if(reply == NULL) return NULL;

  dbus_message_iter_init_append(reply, &iter);
  dbus_message_iter_open_container(&iter, DBUS_TYPE_VARIANT, property->type, &value);

  if(!property->get(property, &value, iface->user_data)) {
    dbus_message_unref(reply);
    return NULL;
  }

  dbus_message_iter_close_container(&iter, &value);

  return reply;
}

static DBusMessage *properties_get_all(DBusConnection *connection,
                                       DBusMessage *   message,
                                       void *          user_data)
{
  dbus_object_t *        data = user_data;
  struct interface_data *iface;
  const char *           interface;
  DBusMessageIter        iter;
  DBusMessage *          reply;

  if(!dbus_message_get_args(message,
                            NULL,
                            DBUS_TYPE_STRING,
                            &interface,
                            DBUS_TYPE_INVALID))
    return NULL;

  if(!find_interface_by_name(data, interface, (void *)&iface)) {
    return dbus_create_error(message,
                             DBUS_ERROR_INVALID_ARGS,
                             "No such interface '%s'",
                             interface);
  }

  reply = dbus_message_new_method_return(message);
  if(reply == NULL) return NULL;

  dbus_message_iter_init_append(reply, &iter);

  append_properties(iface, &iter);

  return reply;
}

static DBusMessage *properties_set(DBusConnection *connection,
                                   DBusMessage *   message,
                                   void *          user_data)
{
  dbus_object_t *          data = user_data;
  DBusMessageIter          iter, sub;
  DBusMessage *            reply;
  struct interface_data *  iface;
  const DBusPropertyTable *property;
  const char *             name, *interface;
  struct property_data *   propdata;
  bool                     valid_signature;
  char *                   signature;

  if(!dbus_message_iter_init(message, &iter))
    return dbus_create_error(message,
                             DBUS_ERROR_INVALID_ARGS,
                             "No arguments given");

  if(dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_STRING)
    return dbus_create_error(message,
                             DBUS_ERROR_INVALID_ARGS,
                             "Invalid argument type: '%c'",
                             dbus_message_iter_get_arg_type(&iter));

  dbus_message_iter_get_basic(&iter, &interface);
  dbus_message_iter_next(&iter);

  if(dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_STRING)
    return dbus_create_error(message,
                             DBUS_ERROR_INVALID_ARGS,
                             "Invalid argument type: '%c'",
                             dbus_message_iter_get_arg_type(&iter));

  dbus_message_iter_get_basic(&iter, &name);
  dbus_message_iter_next(&iter);

  if(dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_VARIANT)
    return dbus_create_error(message,
                             DBUS_ERROR_INVALID_ARGS,
                             "Invalid argument type: '%c'",
                             dbus_message_iter_get_arg_type(&iter));

  dbus_message_iter_recurse(&iter, &sub);

  if(!find_interface_by_name(data, interface, (void *)&iface)) {
    return dbus_create_error(message,
                             DBUS_ERROR_INVALID_ARGS,
                             "No such interface '%s'",
                             interface);
  }

  if(!find_property(iface->properties, name, &property))
    return dbus_create_error(message,
                             DBUS_ERROR_UNKNOWN_PROPERTY,
                             "No such property '%s'",
                             name);

  if(property->set == NULL)
    return dbus_create_error(message,
                             DBUS_ERROR_PROPERTY_READ_ONLY,
                             "Property '%s' is not writable",
                             name);

  if(property->exists != NULL && !property->exists(property, iface->user_data))
    return dbus_create_error(message,
                             DBUS_ERROR_UNKNOWN_PROPERTY,
                             "No such property '%s'",
                             name);

  signature       = dbus_message_iter_get_signature(&sub);
  valid_signature = strcmp(signature, property->type) ? FALSE : TRUE;
  dbus_free(signature);
  if(!valid_signature)
    return dbus_create_error(message,
                             DBUS_ERROR_INVALID_SIGNATURE,
                             "Invalid signature for '%s'",
                             name);

  propdata          = malloc(sizeof(struct property_data));
  propdata->message = message;
  propdata->conn    = connection;

  property->set(property, &sub, propdata->id, iface->user_data);

  reply = dbus_message_new_method_return(message);
  if(reply == NULL) return NULL;

  return reply;
}

void process_property_changes(dbus_object_t *data)
{
  list_head_t *list_head = sets_get_listhead(&data->interfaces);

  data->pending_prop = FALSE;

  struct interface_data *iface;
  list_foreach_data(iface, list_head)
  {
    process_properties_from_interface(data, iface);
  }
}

void process_properties_from_interface(dbus_object_t *        data,
                                       struct interface_data *iface)
{
  DBusMessage *   signal;
  DBusMessageIter iter, dict, array;
  list_head_t     invalidated;

  if(is_sets_empty(&iface->pending_prop)) return;

  signal = dbus_message_new_signal(data->path,
                                   DBUS_INTERFACE_PROPERTIES,
                                   "PropertiesChanged");
  if(signal == NULL) {
    return;
  }

  // clang-format off
  dbus_message_iter_init_append(signal, &iter);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING,	&iface->name);
  dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
                DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
                DBUS_TYPE_STRING_AS_STRING DBUS_TYPE_VARIANT_AS_STRING
                DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &dict);
  // clang-format on

  list_init(&invalidated, NULL);

  DBusPropertyTable *p;
  sets_foreach_data(p, &iface->pending_prop)
  {
    if(p->get == NULL) continue;

    if(p->exists != NULL && !p->exists(p, iface->user_data)) {
      list_add_data_head(&invalidated, p);
      continue;
    }

    append_property(iface, p, &dict);
  }

  dbus_message_iter_close_container(&iter, &dict);

  dbus_message_iter_open_container(&iter,
                                   DBUS_TYPE_ARRAY,
                                   DBUS_TYPE_STRING_AS_STRING,
                                   &array);
  list_foreach_data(p, &invalidated)
  {
    dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &p->name);
  }
  list_cleanup(&invalidated);
  dbus_message_iter_close_container(&iter, &array);

  sets_cleanup(&iface->pending_prop);

  /* Use dbus_connection_send to avoid recursive calls to g_dbus_flush */
  dbus_connection_send(data->conn, signal, NULL);
  dbus_message_unref(signal);
}

static bool find_property(const DBusPropertyTable * property,
                          const char *              name,
                          const DBusPropertyTable **out)
{
  const DBusPropertyTable *find = NULL;
  for(; property && property->name != NULL; property++) {
    if(strcmp(property->name, name) == 0) {
      find = property;
      if(out != NULL) {
        *out = property;
      }
      break;
    }
  }

  return find != NULL;
}