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
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
void dbus_dispatch_status(DBusConnection*    connection,
                          DBusDispatchStatus new_status,
                          void*              data)
{
  switch(new_status) {
    case DBUS_DISPATCH_DATA_REMAINS:
      queue_dispatch(connection);
      break;
    case DBUS_DISPATCH_COMPLETE:
      break;
    case DBUS_DISPATCH_NEED_MEMORY:
    default:
      break;
  }
}
