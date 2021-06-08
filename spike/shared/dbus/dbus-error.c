/** See a brief introduction (right-hand button) */
#include "dbus-error.h"
/* Private include -----------------------------------------------------------*/
#include <stdarg.h>
#include <stdio.h>

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
DBusMessage *dbus_create_error(DBusMessage *message,
                               const char * name,
                               const char * format,
                               ...)
{
  va_list      args;
  DBusMessage *reply;

  va_start(args, format);

  reply = dbus_create_error_valist(message, name, format, args);

  va_end(args);

  return reply;
}

DBusMessage *dbus_create_error_valist(DBusMessage *message,
                                      const char * name,
                                      const char * format,
                                      va_list      args)
{
  char str[1024];

  /* Check if the message can be replied */
  if(dbus_message_get_no_reply(message)) return NULL;

  if(format)
    vsnprintf(str, sizeof(str), format, args);
  else
    str[0] = '\0';

  return dbus_message_new_error(message, name, str);
}
