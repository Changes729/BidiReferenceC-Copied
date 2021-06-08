/** See a brief introduction (right-hand button) */
#include "dbus_initial.h"
/* Private include -----------------------------------------------------------*/
#include <dbus/dbus.h>

#include "dbus-task.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
extern dbus_bool_t add_timeout(DBusTimeout *t, void *data);
extern void        remove_timeout(DBusTimeout *t, void *data);
extern void        toggle_timeout(DBusTimeout *t, void *data);

extern dbus_bool_t add_watch(DBusWatch *, void *data);
extern void        remove_watch(DBusWatch *, void *data);
extern void        toggled_watch(DBusWatch *watch, void *data);

extern void dbus_dispatch_status(DBusConnection *   connection,
                                 DBusDispatchStatus new_status,
                                 void *             data);
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */
bool dbus_init(DBusBusType      type,
               const char *     dbus_name,
               unsigned int     flag,
               DBusConnection **connection)
{
  DBusConnection *conn;
  DBusError       err;

  dbus_error_init(&err);

  conn = dbus_bus_get_private(type, &err);
  if(dbus_error_is_set(&err)) {
    dbus_error_free(&err);
    goto __failed;
  }

  dbus_bus_request_name(conn, dbus_name, flag, &err);
  if(dbus_error_is_set(&err)) {
    dbus_error_free(&err);
    goto __failed;
  }

  if(!dbus_connection_set_watch_functions(conn,
                                          add_watch,
                                          remove_watch,
                                          toggled_watch,
                                          conn,
                                          NULL))
  {
    goto __failed;
  }

  if(!dbus_connection_set_timeout_functions(conn,
                                            add_timeout,
                                            remove_timeout,
                                            toggle_timeout,
                                            NULL,
                                            NULL))
  {
    goto __failed;
  }

  message_dispatch(conn);
  dbus_connection_set_dispatch_status_function(conn,
                                               dbus_dispatch_status,
                                               NULL,
                                               NULL);

  dbus_connection_set_exit_on_disconnect(conn, FALSE);
  if(connection != NULL) {
    *connection = conn;
  }

  return true;

__failed:
  dbus_final(conn);
  return false;
}

void dbus_final(DBusConnection *connection)
{
  if(connection == NULL) {
    return;
  }

  dbus_connection_set_dispatch_status_function(connection, NULL, NULL, NULL);

  dbus_connection_flush(connection);
  dbus_connection_close(connection);
  dbus_connection_unref(connection);
}
