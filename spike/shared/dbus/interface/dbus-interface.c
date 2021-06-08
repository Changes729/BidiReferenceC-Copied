/** See a brief introduction (right-hand button) */
#include "dbus-interface.h"
/* Private include -----------------------------------------------------------*/
#include <string.h>

#include "dbus-interface.h"
#include "dbus-object.h"
#include "dbus-task.h"
#include "properties/properties.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
bool find_interface_method(struct dbus_object *dbus_object,
                           const char *        iface_name,
                           const char *        method_name,
                           DBusMethodFunction *method)
{
  list_head_t *list_head = sets_get_listhead(&dbus_object->interfaces);
  bool         find      = false;

  struct interface_data *interface;
  list_foreach_data(interface, list_head)
  {
    if(strcmp(interface->name, iface_name) != 0) {
      continue;
    }

    for(const DBusMethodTable *node = interface->methods; NULL != node->name;
        node++)
    {
      if(strcmp(node->name, method_name) == 0) {
        find = true;
        if(method != NULL) {
          *method = node->function;
        }
        break;
      }
    }
  }

  return find;
}

interface_data_t *add_interface(dbus_object_t *          data,
                                const char *             name,
                                const DBusMethodTable *  methods,
                                const DBusSignalTable *  signals,
                                const DBusPropertyTable *properties,
                                void *                   user_data,
                                DBusDestroyFunction      destroy)
{
  interface_data_t *iface    = NULL;
  char *            name_dup = NULL;
  if(find_interface_by_name(data, name, &iface) == true) {
    goto __end;
  }

  name_dup = strdup(name);
  iface    = malloc(sizeof(interface_data_t));
  if(name_dup == NULL || iface == NULL) {
    goto __failed;
  }

  memset(iface, 0, sizeof(interface_data_t));
  iface->name       = name_dup;
  iface->methods    = methods;
  iface->signals    = signals;
  iface->properties = properties;
  iface->user_data  = user_data;
  iface->destroy    = destroy;

  sets_add(&data->interfaces, iface);
  if(sets_find(&data->interfaces, iface, NULL) == true) {
    free(data->introspect);
    data->introspect = NULL;
    sets_add(&data->added, iface);

    queue_pading(data);
    // todo: notify interfaceAdd.
    goto __end;
  }

__failed:
  free(name_dup);
  free(iface);
  iface = NULL;

__end:
  return iface;
}

void remove_interface(dbus_object_t *data, const char *name)
{
  interface_data_t *iface = NULL;
  if(find_interface_by_name(data, name, &iface) == false) {
    goto __end;
  }

  process_properties_from_interface(data, iface);
  sets_remove(&data->interfaces, iface);

  if(iface->destroy) {
    iface->destroy(iface->user_data);
    iface->user_data = NULL;
  }

  if(sets_find(&data->added, iface, NULL)) {
    sets_remove(&data->added, iface);
    goto __clean;
  }

  if(data->parent == NULL) {
    goto __clean;
  }

  sets_add(&data->removed, iface->name);
  iface->name = NULL;

__clean:
  free(iface->name);
  free(iface);

__end:
  queue_pading(data);
  return;
}

bool find_interface_by_name(struct dbus_object *    dbus_object,
                            const char *            name,
                            struct interface_data **interface)
{
  const struct interface_data find_tmp = {.name = (char *)name};
  return sets_find(&dbus_object->interfaces, &find_tmp, (void *)interface);
}
