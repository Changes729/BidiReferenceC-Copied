/** See a brief introduction (right-hand button) */
#include "dbus_root_object.h"
/* Private include -----------------------------------------------------------*/
#include "dbus_define.h"
#include "dbus-object.h"
#include "introspectable/introspectable.h"
#include "object-manager/object-manager.h"
#include "properties/properties.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct dbus_object root = {
    .path = "/",
};

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
int register_root_object(DBusConnection *connection)
{
  struct dbus_object *data = attach_dbus_object(connection, root.path);
  if(data == NULL) return FALSE;

  introspectable_regist(data);
  object_manager_regist(data);

  return TRUE;
}

void unregister_root_object(DBusConnection *connection)
{
  dbus_object_t *dbus_object;
  if(!dbus_connection_get_object_path_data(connection,
                                           root.path,
                                           (void *)&dbus_object) ||
     dbus_object == NULL)
  {
    return;
  }

  introspectable_unregist(dbus_object);
  object_manager_unregist(dbus_object);
  detach_dbus_object(connection, root.path);
}