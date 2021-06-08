/** See a brief introduction (right-hand button) */
#include "object-manager.h"
/* Private include -----------------------------------------------------------*/
#include "dbus-interface.h"
#include "properties/properties.h"

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static DBusMessage *get_objects(DBusConnection *connection,
                                DBusMessage *   message,
                                void *          user_data);

static int  append_object(void *data, void *user_data);
static void append_interfaces(struct dbus_object *data, DBusMessageIter *iter);
static int  append_interface(void *data, void *user_data);
static int  append_name(void *data, void *user_data);

/* Private variables ---------------------------------------------------------*/
static DBusMethodTable object_methods[] = {
    {DBUS_METHOD("GetManagedObjects",
                 NULL,
                 DBUS_ARGS({"objects", "a{oa{sa{sv}}}"}),
                 get_objects)},
    {},
};

static const DBusSignalTable object_signals[] = {
    {DBUS_SIGNAL("InterfacesAdded",
                 DBUS_ARGS({"object", "o"}, {"interfaces", "a{sa{sv}}"}))},
    {DBUS_SIGNAL("InterfacesRemoved",
                 DBUS_ARGS({"object", "o"}, {"interfaces", "as"}))},
    {},
};

/* Private function ----------------------------------------------------------*/
interface_data_t *object_manager_regist(dbus_object_t *data)
{
  return add_interface(data,
                       DBUS_INTERFACE_OBJECT_MANAGER,
                       object_methods,
                       object_signals,
                       NULL,
                       data,
                       NULL);
}

void object_manager_unregist(dbus_object_t *data)
{
  remove_interface(data, DBUS_INTERFACE_OBJECT_MANAGER);
}

static DBusMessage *get_objects(DBusConnection *connection,
                                DBusMessage *   message,
                                void *          user_data)
{
  struct dbus_object *data = user_data;
  DBusMessage *       reply;
  DBusMessageIter     iter;
  DBusMessageIter     array;

  reply = dbus_message_new_method_return(message);
  if(reply == NULL) return NULL;

  dbus_message_iter_init_append(reply, &iter);

  // clang-format off
	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
					DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
					DBUS_TYPE_OBJECT_PATH_AS_STRING
					DBUS_TYPE_ARRAY_AS_STRING
					DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
					DBUS_TYPE_STRING_AS_STRING
					DBUS_TYPE_ARRAY_AS_STRING
					DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
					DBUS_TYPE_STRING_AS_STRING
					DBUS_TYPE_VARIANT_AS_STRING
					DBUS_DICT_ENTRY_END_CHAR_AS_STRING
					DBUS_DICT_ENTRY_END_CHAR_AS_STRING
					DBUS_DICT_ENTRY_END_CHAR_AS_STRING,
					&array);
  // clang-format on

  sets_foreach(&data->objects, append_object, &array);

  dbus_message_iter_close_container(&iter, &array);

  return reply;
}

void emit_interfaces_added(struct dbus_object *data)
{
  DBusMessage *       signal;
  DBusMessageIter     iter, array;
  struct dbus_object *root = data->parent;
  if(data->parent == NULL) return;
  while(root->parent != NULL) {
    root = root->parent;
  }

  signal = dbus_message_new_signal(root->path,
                                   DBUS_INTERFACE_OBJECT_MANAGER,
                                   "InterfacesAdded");
  if(signal == NULL) return;

  dbus_message_iter_init_append(signal, &iter);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_OBJECT_PATH, &data->path);

  // clang-format off
	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
				DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				DBUS_TYPE_STRING_AS_STRING
				DBUS_TYPE_ARRAY_AS_STRING
				DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				DBUS_TYPE_STRING_AS_STRING
				DBUS_TYPE_VARIANT_AS_STRING
				DBUS_DICT_ENTRY_END_CHAR_AS_STRING
				DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &array);
  // clang-format on

  sets_foreach(&data->added, append_interface, &array);
  sets_cleanup(&data->added);

  dbus_message_iter_close_container(&iter, &array);

  /* Use dbus_connection_send to avoid recursive calls to g_dbus_flush */
  dbus_connection_send(data->conn, signal, NULL);
  dbus_message_unref(signal);
}

void emit_interfaces_removed(struct dbus_object *data)
{
  DBusMessage *       signal;
  DBusMessageIter     iter, array;
  struct dbus_object *root = data->parent;
  if(root == NULL) return;
  while(root->parent != NULL) {
    root = root->parent;
  }

  signal = dbus_message_new_signal(root->path,
                                   DBUS_INTERFACE_OBJECT_MANAGER,
                                   "InterfacesRemoved");
  if(signal == NULL) return;

  dbus_message_iter_init_append(signal, &iter);
  dbus_message_iter_append_basic(&iter, DBUS_TYPE_OBJECT_PATH, &data->path);
  dbus_message_iter_open_container(&iter,
                                   DBUS_TYPE_ARRAY,
                                   DBUS_TYPE_STRING_AS_STRING,
                                   &array);

  sets_foreach(&data->removed, append_name, &array);
  sets_cleanup(&data->removed);

  dbus_message_iter_close_container(&iter, &array);

  /* Use dbus_connection_send to avoid recursive calls to g_dbus_flush */
  dbus_connection_send(data->conn, signal, NULL);
  dbus_message_unref(signal);
}

static int append_object(void *data, void *user_data)
{
  struct dbus_object *child = data;
  DBusMessageIter *   array = user_data;
  DBusMessageIter     entry;

  dbus_message_iter_open_container(array, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
  dbus_message_iter_append_basic(&entry, DBUS_TYPE_OBJECT_PATH, &child->path);
  append_interfaces(child, &entry);
  dbus_message_iter_close_container(array, &entry);

  sets_foreach(&child->objects, append_object, user_data);

  return true;
}

static void append_interfaces(struct dbus_object *data, DBusMessageIter *iter)
{
  DBusMessageIter array;

  // clang-format off
  dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
          DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
          DBUS_TYPE_STRING_AS_STRING
          DBUS_TYPE_ARRAY_AS_STRING
          DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
          DBUS_TYPE_STRING_AS_STRING
          DBUS_TYPE_VARIANT_AS_STRING
          DBUS_DICT_ENTRY_END_CHAR_AS_STRING
          DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &array);
  // clang-format on

  sets_foreach(&data->interfaces, append_interface, &array);

  dbus_message_iter_close_container(iter, &array);
}

static int append_interface(void *data, void *user_data)
{
  struct interface_data *iface = data;
  DBusMessageIter *      array = user_data;
  DBusMessageIter        entry;

  dbus_message_iter_open_container(array, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
  dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &iface->name);
  append_properties(data, &entry);
  dbus_message_iter_close_container(array, &entry);

  return true;
}

static int append_name(void *data, void *user_data)
{
  char *           name = data;
  DBusMessageIter *iter = user_data;

  dbus_message_iter_append_basic(iter, DBUS_TYPE_STRING, &name);

  return true;
}
