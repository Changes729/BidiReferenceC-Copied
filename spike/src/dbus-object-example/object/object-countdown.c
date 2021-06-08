/** See a brief introduction (right-hand button) */
#include "object-countdown.h"
/* Private include -----------------------------------------------------------*/
#include <stdio.h>

#include "dbus-interface.h"
#include "dbus-object.h"
#include "dbus_define.h"
#include "introspectable/introspectable.h"
#include "object-manager/object-manager.h"
#include "properties/properties.h"
#include "timer-task.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DBUS_INTERFACE_COUNTDOWN "org.freedesktop.DBus.Countdown"

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  DBusConnection *connection;
  int             count_down;
  const char *    info;
  timer_task_t *  task;
} _count_down_t;

/* Private template ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static bool get_countdown(const DBusPropertyTable *property,
                          DBusMessageIter *        iter,
                          void *                   data);
static void set_countdown(const DBusPropertyTable *property,
                          DBusMessageIter *        value,
                          DBusPendingPropertySet   id,
                          void *                   data);
static bool countdown_exists(const DBusPropertyTable *property, void *data);
static bool get_information(const DBusPropertyTable *property,
                            DBusMessageIter *        iter,
                            void *                   data);

interface_data_t *countdown_regist(dbus_object_t *data);
void              countdown_unregist(dbus_object_t *data);

static void countdown(void *data);
static void countdown_start(void *data);
static void countdown_cleanup(_count_down_t *p);

/* Private variables ---------------------------------------------------------*/
static struct dbus_object root = {
    .path = "/org/countdown",
};

// dbus-send --session --dest=org.gnome.ServiceName --type=method_call --print-reply=literal --reply-timeout=1000 /org/countdown org.freedesktop.DBus.Properties.Set string:"org.freedesktop.DBus.Countdown" string:"Count" variant:int32:"10"
static const DBusPropertyTable countdown_properties[] =
    {{"Count", "i", get_countdown, set_countdown, countdown_exists},
     {"Information", "s", get_information},
     {}};

static _count_down_t count_down;

/* Private function ----------------------------------------------------------*/
int register_countdown_object(DBusConnection *connection)
{
  struct dbus_object *data = attach_dbus_object(connection, root.path);
  if(data == NULL) return FALSE;

  introspectable_regist(data);
  properties_regist(data);
  countdown_regist(data);

  count_down.connection = connection;
  count_down.count_down = 10; /* s */
  count_down.info       = "Hello world!";

  return TRUE;
}

void unregister_countdown_object(DBusConnection *connection)
{
  dbus_object_t *dbus_object;
  if(!dbus_connection_get_object_path_data(connection,
                                           root.path,
                                           (void *)&dbus_object) ||
     dbus_object == NULL)
  {
    return;
  }

  countdown_unregist(dbus_object);
  introspectable_unregist(dbus_object);
  properties_unregist(dbus_object);
  detach_dbus_object(connection, root.path);
}

interface_data_t *countdown_regist(dbus_object_t *data)
{
  if(count_down.task == NULL)
    count_down.task = timer_task_new(1000, countdown, &count_down);

  return add_interface(data,
                       DBUS_INTERFACE_COUNTDOWN,
                       NULL,
                       NULL,
                       countdown_properties,
                       &count_down,
                       (DBusDestroyFunction)countdown_cleanup);
}

void countdown_unregist(dbus_object_t *data)
{
  remove_interface(data, DBUS_INTERFACE_COUNTDOWN);
}

static bool get_countdown(const DBusPropertyTable *property,
                          DBusMessageIter *        iter,
                          void *                   data)
{
  _count_down_t *desc = data;

  dbus_message_iter_append_basic(iter, DBUS_TYPE_INT32, &desc->count_down);

  return TRUE;
}

static void set_countdown(const DBusPropertyTable *property,
                          DBusMessageIter *        value,
                          DBusPendingPropertySet   id,
                          void *                   data)
{
  _count_down_t *desc = data;

  if(dbus_message_iter_get_arg_type(value) != DBUS_TYPE_INT32) {
    // g_dbus_pending_property_error(id,
    //                               "org.bluez.InvalidArguments",
    //                               "Invalid arguments in method call");
    return;
  }

  dbus_message_iter_get_basic(value, &desc->count_down);
}

static bool countdown_exists(const DBusPropertyTable *property, void *data)
{
  _count_down_t *desc = data;

  return desc->info != NULL;  // true
}

static bool get_information(const DBusPropertyTable *property,
                            DBusMessageIter *        iter,
                            void *                   data)
{
  _count_down_t *desc = data;

  dbus_message_iter_append_basic(iter, DBUS_TYPE_STRING, &desc->info);

  return TRUE;
}

static void countdown(void *data)
{
  _count_down_t *desc = data;

  desc->count_down--;
  emit_property_changed(desc->connection,
                        root.path,
                        DBUS_INTERFACE_COUNTDOWN,
                        "Count");

  printf("\t countdown. %d", desc->count_down);
  fflush(stdout);

  if(desc->count_down > 0) {
    timer_task_continue(desc->task);
  } else {
    desc->task = NULL;
    unregister_countdown_object(desc->connection);
    timer_task_new(3000, countdown_start, desc->connection);
  }
}

void countdown_start(void *data)
{
  DBusConnection *connection = data;

  printf("\r\n register countdown");
  fflush(stdout);

  register_countdown_object(connection);
}

static void countdown_cleanup(_count_down_t *p)
{
  timer_task_del(p->task);
  p->task = NULL;
}
