/** See a brief introduction (right-hand button) */
#include "dbus_initial.h"
/* Private include -----------------------------------------------------------*/
#include <dbus/dbus.h>

#include "timer-task.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void handle_timeout(void *x);

/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
dbus_bool_t add_timeout(DBusTimeout *t, void *data)
{
  timer_task_t *timer_task = NULL;

  if(!dbus_timeout_get_enabled(t)) return TRUE;

  timer_task = timer_task_new(dbus_timeout_get_interval(t), handle_timeout, t);

  dbus_timeout_set_data(t, timer_task, NULL);

  return TRUE;
}

void remove_timeout(DBusTimeout *t, void *data)
{
  timer_task_t *timer_task = dbus_timeout_get_data(t);
  timer_task_del(timer_task);
  dbus_timeout_set_data(t, NULL, NULL);
}

void toggle_timeout(DBusTimeout *t, void *data)
{
  if(dbus_timeout_get_enabled(t))
    add_timeout(t, data);
  else
    remove_timeout(t, data);
}

static void handle_timeout(void *x)
{
  DBusTimeout *t = x;

  dbus_timeout_handle(t);
}
