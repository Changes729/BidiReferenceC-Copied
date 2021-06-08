/** See a brief introduction (right-hand button) */
#include "dbus-task.h"
/* Private include -----------------------------------------------------------*/
#include "object-manager/object-manager.h"
#include "properties/properties.h"
#include "set-list.h"
#include "timer-task.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  void *        key;
  timer_task_t *task;
} timer_map_t;

/* Private template ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static timer_map_t *new_timer_map(void *key, timer_task_t *task);
static int          timer_map_cmp(const timer_map_t *, const timer_map_t *);
static void         free_timer_map(timer_map_t *p);

/* Private variables ---------------------------------------------------------*/
static timer_task_t *_queue_dispatch_task = NULL;
sets_t _queue_object_update_tasks = {.cb_cmp = (CompareCallback_t)timer_map_cmp,
                                     .cb_cleanup = (cleanup_cb_t)free_timer_map};

/* Private class -------------------------------------------------------------*/
/* Private function ----------------------------------------------------------*/
static timer_map_t *new_timer_map(void *key, timer_task_t *task)
{
  timer_map_t *data = malloc(sizeof(timer_map_t));

  if(data != NULL) {
    data->key  = key;
    data->task = task;
  }

  return data;
}

static int timer_map_cmp(const timer_map_t *a, const timer_map_t *b)
{
  return (intptr_t)a->key - (intptr_t)b->key;
}

static void free_timer_map(timer_map_t *p)
{
  if(p != NULL) {
    timer_task_del(p->task);
  }
  free(p);
}

/* Private class function ----------------------------------------------------*/
void queue_dispatch(DBusConnection *connection)
{
  if(dbus_connection_get_dispatch_status(connection) == DBUS_DISPATCH_DATA_REMAINS)
    if(_queue_dispatch_task == NULL)
      _queue_dispatch_task = timer_task_new(0, message_dispatch, connection);
}

void queue_pading(dbus_object_t *data)
{
  timer_map_t find = {data, NULL};
  if(!sets_find(&_queue_object_update_tasks, &find, NULL)) {
    timer_task_t *task = timer_task_new(0, process_pading, data);
    sets_add(&_queue_object_update_tasks, new_timer_map(data, task));
  }
}

void message_dispatch(void *data)
{
  DBusConnection *connection = data;

  if(dbus_connection_get_dispatch_status(connection) == DBUS_DISPATCH_COMPLETE) {
    timer_task_del(_queue_dispatch_task);
    _queue_dispatch_task = NULL;
    return;
  }

  while(dbus_connection_dispatch(connection) == DBUS_DISPATCH_DATA_REMAINS) {
  }

  timer_task_del(_queue_dispatch_task);
  _queue_dispatch_task = NULL;
}

void process_pading(void *user_data)
{
  struct dbus_object *data = user_data;

  if(sets_length(&data->added) > 0) emit_interfaces_added(data);

  /* Flush pending properties */
  if(data->pending_prop == TRUE) process_property_changes(data);

  if(sets_length(&data->removed) > 0) emit_interfaces_removed(data);

  timer_map_t remove = {data, NULL};
  sets_remove(&_queue_object_update_tasks, &remove);
}