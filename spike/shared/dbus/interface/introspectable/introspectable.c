/** See a brief introduction (right-hand button) */
#include "introspectable.h"
/* Private include -----------------------------------------------------------*/
#include "dbus-interface.h"
#include "string-buffer.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DBUS_ANNOTATE(name_, value_)                                           \
  "<annotation name=\"org.freedesktop.DBus." name_ "\"value=\"" value_ "\"/>"

#define DBUS_ANNOTATE_DEPRECATED DBUS_ANNOTATE("Deprecated", "true")
#define DBUS_ANNOTATE_NOREPLY    DBUS_ANNOTATE("Method.NoReply", "true")

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static DBusMessage *introspect(DBusConnection *, DBusMessage *, void *user_data);

static void _generate_introspection_xml(DBusConnection *    conn,
                                        struct dbus_object *data,
                                        const char *        path);
static void _generate_interface_xml(struct string *        str,
                                    struct interface_data *iface);
static void _print_arguments(struct string *    str,
                             const DBusArgInfo *args,
                             const char *       direction);

/* Private variables ---------------------------------------------------------*/
static DBusMethodTable introspect_methods[] = {
    {DBUS_METHOD("Introspect", NULL, DBUS_ARGS({"xml", "s"}), introspect)},
    {},
};

/* Private function ----------------------------------------------------------*/
interface_data_t *introspectable_regist(dbus_object_t *data)
{
  return add_interface(data,
                       DBUS_INTERFACE_INTROSPECTABLE,
                       introspect_methods,
                       NULL,
                       NULL,
                       data,
                       NULL);
}

void introspectable_unregist(dbus_object_t *data)
{
  remove_interface(data, DBUS_INTERFACE_INTROSPECTABLE);
}

static DBusMessage *introspect(DBusConnection *connection,
                               DBusMessage *   message,
                               void *          user_data)
{
  struct dbus_object *data  = user_data;
  DBusMessage *       reply = NULL;

  reply = dbus_message_new_method_return(message);
  if(reply == NULL) {
    goto end;
  }

  if(NULL == data->introspect) {
    _generate_introspection_xml(connection, data, dbus_message_get_path(message));
  }

  if(NULL != data->introspect) {
    dbus_message_append_args(reply,
                             DBUS_TYPE_STRING,
                             &data->introspect,
                             DBUS_TYPE_INVALID);
  } else {
    dbus_message_unref(reply);
    reply = NULL;
  }

end:
  return reply;
}

static void _generate_introspection_xml(DBusConnection *    conn,
                                        struct dbus_object *data,
                                        const char *        path)
{
  struct string *str;
  char **        children __attribute__((unused));

  free(data->introspect);

  str = string_new(DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE);

  string_append_printf(str, "<node>");

  list_foreach(list, sets_get_listhead(&data->interfaces))
  {
    struct interface_data *iface = list->data;
    string_append_printf(str, "<interface name=\"%s\">", iface->name);

    _generate_interface_xml(str, iface);

    string_append_printf(str, "</interface>");
  }

  if(!dbus_connection_list_registered(conn, path, &children)) goto done;

  for(size_t i = 0; children[i]; i++)
    string_append_printf(str, "<node name=\"%s\"/>", children[i]);

  dbus_free_string_array(children);

done:
  string_append_printf(str, "</node>");

  data->introspect = strdup(string_c_str(str));
  string_delete(str);
}

static void _generate_interface_xml(struct string *        str,
                                    struct interface_data *iface)
{
  const DBusMethodTable *  method;
  const DBusSignalTable *  signal;
  const DBusPropertyTable *property;

  for(method = iface->methods; method && method->name; method++) {
    string_append_printf(str, "<method name=\"%s\">", method->name);
    _print_arguments(str, method->in_args, "in");
    _print_arguments(str, method->out_args, "out");

    if(method->flags & G_DBUS_METHOD_FLAG_DEPRECATED)
      string_append_printf(str, DBUS_ANNOTATE_DEPRECATED);

    if(method->flags & G_DBUS_METHOD_FLAG_NOREPLY)
      string_append_printf(str, DBUS_ANNOTATE_NOREPLY);

    string_append_printf(str, "</method>");
  }

  for(signal = iface->signals; signal && signal->name; signal++) {
    string_append_printf(str, "<signal name=\"%s\">", signal->name);
    _print_arguments(str, signal->args, NULL);

    if(signal->flags & G_DBUS_SIGNAL_FLAG_DEPRECATED)
      string_append_printf(str, DBUS_ANNOTATE_DEPRECATED);

    string_append_printf(str, "</signal>");
  }

  for(property = iface->properties; property && property->name; property++) {
    string_append_printf(str,
                         "<property name=\"%s\""
                         " type=\"%s\" access=\"%s%s\">",
                         property->name,
                         property->type,
                         property->get ? "read" : "",
                         property->set ? "write" : "");

    if(property->flags & G_DBUS_PROPERTY_FLAG_DEPRECATED)
      string_append_printf(str, DBUS_ANNOTATE_DEPRECATED);

    string_append_printf(str, "</property>");
  }
}

static void _print_arguments(struct string *    str,
                             const DBusArgInfo *args,
                             const char *       direction)
{
  for(; args && args->name; args++) {
    string_append_printf(str,
                         "<arg name=\"%s\" type=\"%s\"",
                         args->name,
                         args->signature);

    if(direction)
      string_append_printf(str, " direction=\"%s\"/>", direction);
    else
      string_append_printf(str, "/>");
  }
}
