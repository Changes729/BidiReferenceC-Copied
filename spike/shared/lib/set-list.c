/** See a brief introduction (right-hand button) */
#include "set-list.h"
/* Private include -----------------------------------------------------------*/
/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static int inline _point_cmp(const void* a, const void* b)
{
  return (uintptr_t)a - (uintptr_t)b;
}

static void inline _cleanup_empty(void* p) {}

/* Private function ----------------------------------------------------------*/
sets_t* sets_alloc(CompareCallback_t cmp, cleanup_cb_t cleanup)
{
  return sets_init(malloc(sizeof(sets_t)), cmp, cleanup);
}

void sets_free(sets_t* set)
{
  free(set);
}

sets_t* sets_init(sets_t* set, CompareCallback_t cmp, cleanup_cb_t cleanup)
{
  if(set == NULL) return set;

  list_init(&set->list_head, set);
  set->count      = 0;
  set->cb_cmp     = cmp;
  set->cb_cleanup = cleanup;

  return set;
}

sets_t* sets_add(sets_t* set, void* data)
{
  if(set == NULL) set = sets_alloc(NULL, NULL);
  if(set == NULL) return set;

  if(!sets_find(set, data, NULL)) {
    if(list_append(&set->list_head, data, NULL) != NULL) {
      set->count++;
    }
  }

  return set;
}

void sets_remove(sets_t* set, void* data)
{
  list_t*           remove = NULL;
  CompareCallback_t cb     = (set->cb_cmp == NULL ? _point_cmp : set->cb_cmp);
  cleanup_cb_t      cleanup =
      (set->cb_cleanup == NULL ? _cleanup_empty : set->cb_cleanup);

  list_foreach(node, &set->list_head)
  {
    if(cb(data, node->data) == 0) {
      remove = node;
      break;
    }
  }

  if(remove != NULL) {
    cleanup(list_node_remove(remove));
    set->count--;
  }
}

void sets_cleanup(sets_t* set)
{
  list_t* node = NULL;

  cleanup_cb_t cleanup =
      (set->cb_cleanup == NULL ? _cleanup_empty : set->cb_cleanup);

  while(NULL != (node = list_get_first(&set->list_head))) {
    cleanup(list_node_remove(node));
    set->count--;
  }
}

bool sets_find(const sets_t* set, const void* data, void** out)
{
  bool              find = false;
  CompareCallback_t cb   = (set->cb_cmp == NULL ? _point_cmp : set->cb_cmp);

  list_foreach(node, &set->list_head)
  {
    if(cb(data, node->data) == 0) {
      if(out != NULL) {
        *out = node->data;
      }
      find = true;
      break;
    }
  }

  return find;
}

unsigned sets_length(const sets_t* set)
{
  return set == NULL ? 0 : set->count;
}

void sets_foreach(sets_t* set, Function_t callback, void* userdata)
{
  list_foreach(node, &set->list_head)
  {
    if(!callback(node->data, userdata)) {
      break;
    }
  }
}

list_head_t* sets_get_listhead(sets_t* sets)
{
  return &sets->list_head;
}
