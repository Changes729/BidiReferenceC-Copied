/** See a brief introduction (right-hand button) */
#include "dbus_initial.h"
/* Private include -----------------------------------------------------------*/
#include <dbus/dbus.h>
#include <stdio.h>
#include <string.h>

#include "dbus-task.h"
#include "io-flush.h"

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
void on_dbus_watch(void *user_data, fd_desc_t pkg)
{
  DBusWatch *     watch      = user_data;
  DBusConnection *connection = dbus_watch_get_data(watch);
  unsigned int    flags      = 0;

  if(pkg.flag & IO_NOTICE_READ) flags |= DBUS_WATCH_READABLE;
  if(pkg.flag & IO_NOTICE_WRITE) flags |= DBUS_WATCH_WRITABLE;
  if(pkg.flag & IO_NOTICE_HUP) flags |= DBUS_WATCH_HANGUP;
  if(pkg.flag & IO_NOTICE_ERR) flags |= DBUS_WATCH_ERROR;

  while(!dbus_watch_handle(watch, flags)) {
    printf("dbus_watch_handle needs more memory\n");
    break;
  }

  queue_dispatch(connection);
}

dbus_bool_t add_watch(DBusWatch *watch, void *data)
{
  unsigned  flags = dbus_watch_get_flags(watch);
  fd_desc_t desc  = {.fd = dbus_watch_get_unix_fd(watch), .flag = 0};

  if(flags & DBUS_WATCH_READABLE) desc.flag |= IO_NOTICE_READ;
  if(flags & DBUS_WATCH_WRITABLE) desc.flag |= IO_NOTICE_WRITE;

  dbus_watch_set_data(watch, data, NULL);
  io_notice_file(desc, on_dbus_watch, watch);

  return 1;
}

void remove_watch(DBusWatch *watch, void *data)
{
  dbus_watch_set_data(watch, NULL, NULL);
  io_ignore_file(dbus_watch_get_unix_fd(watch));
}

void toggled_watch(DBusWatch *watch, void *data)
{
  if(dbus_watch_get_data(watch)) {
    add_watch(watch, data);
  } else {
    remove_watch(watch, data);
  }
}
