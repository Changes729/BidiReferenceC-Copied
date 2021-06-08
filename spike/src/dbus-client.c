#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus.h>
#include <unistd.h>

#include "dbus_define.h"

static void receive_signal_info(DBusMessage *msg, DBusConnection *conn);
static int method_call(DBusMessage *msg, DBusConnection *conn);

int main(int agrc, char **argv)
{
  DBusError err;
  DBusConnection *dbus_address;

  dbus_error_init(&err);

  dbus_address = dbus_bus_get(DBUS_BUS_SESSION, &err);
  if (dbus_error_is_set(&err))
  {
    fprintf(stderr, "Connection Error (%s)\n", err.message);
    dbus_error_free(&err);
    goto __failed;
  }

  dbus_bus_request_name(dbus_address, TEST_DBUS_BUS_NAME, DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &err);
  if (dbus_error_is_set(&err))
  {
    fprintf(stderr, "Name Error (%s)\n", err.message);
    dbus_error_free(&err);
    goto __failed;
  }

  dbus_bus_add_match(dbus_address, "type='signal',"
                                   "sender='" TEST_DBUS_NODE "',"
                                   "interface='" TEST_DBUS_INTERFACE_BASE "'",
                     &err);

  DBusMessage *msg;
  while (TRUE)
  {
    // non blocking read of the next available message
    dbus_connection_read_write(dbus_address, 0);
    msg = dbus_connection_pop_message(dbus_address);

    if (NULL == msg)
    {
      sleep(1);
      method_call(NULL, dbus_address);
      continue;
    }

    if (dbus_message_is_signal(msg, TEST_DBUS_INTERFACE_BASE, "Signal"))
    {
      receive_signal_info(msg, dbus_address);
    }
  }

__failed:
  return 0;
}

static void receive_signal_info(DBusMessage *msg, DBusConnection *conn)
{
  DBusMessageIter iter;
  char *param = "";

  if (!dbus_message_iter_init(msg, &iter))
    fprintf(stderr, "Message has no arguments!\n");
  else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&iter))
    fprintf(stderr, "Argument is not string!\n");
  else
    dbus_message_iter_get_basic(&iter, &param);

  printf("Signal called with %s\n", param);
}

static int method_call(DBusMessage *msg, DBusConnection *conn)
{
  DBusPendingCall *pending = NULL;
  DBusMessageIter iter;
  dbus_uint32_t level;
  dbus_bool_t _bool;

  msg = dbus_message_new_method_call(TEST_DBUS_BUS_NAME, TEST_DBUS_OBJECT, TEST_DBUS_INTERFACE_BASE, "Method");
  if (NULL == msg)
  {
    printf("Message Null");
    return 1;
  }

  const char* str = "Hello world";

  dbus_message_iter_init_append(msg, &iter);
  if (!dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &str))
  {
    fprintf(stderr, "Out of memory\n");
    return 1;
  }

  if (!dbus_connection_send_with_reply(conn, msg, &pending, 1000))
  {
    fprintf(stderr, "Out of memory");
    return 1;
  }

  dbus_connection_flush(conn);
  dbus_message_unref(msg);

  dbus_pending_call_block(pending);
  msg = dbus_pending_call_steal_reply(pending);
  if (NULL == msg)
  {
    printf("Message Null");
    return 1;
  }
  dbus_pending_call_unref(pending);

  if (!dbus_message_iter_init(msg, &iter))
  {
    fprintf(stderr, "Message has no arguments!\n");
  }
  else if (DBUS_TYPE_BOOLEAN != dbus_message_iter_get_arg_type(&iter))
  {
    fprintf(stderr, "Argument is not boolean!\n");
  }
  else
  {
    dbus_message_iter_get_basic(&iter, &_bool);
  }

  if (!dbus_message_iter_next(&iter))
  {
    fprintf(stderr, "Message has too few arguments!\n");
  }
  else if (DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&iter))
  {
    fprintf(stderr, "Argument is not int!\n");
  }
  else
  {
    dbus_message_iter_get_basic(&iter, &level);
    printf("Get method cal return-- status:%d, level:%d\n", _bool, level);
  }

  dbus_message_unref(msg);
}