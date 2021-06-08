/** See a brief introduction (right-hand button) */
#include "glike-list.h"
/* Private include -----------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>

#include "list-operation.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static list_t *_handle_list_init_empty(list_t *list);
static bool    _is_node_in_list(const list_t *node, const list_head_t *list);

/* Private function ----------------------------------------------------------*/
inline list_head_t *list_alloc(void *data)
{
  return list_init(malloc(sizeof(struct list)), data);
}

inline list_head_t *list_init(list_head_t *list, void *data)
{
  if(NULL == list) return list;

  INIT_LIST_HEAD(&list->head);
  list->data = data;
  return list;
}

inline void list_free(list_head_t *list)
{
  free(list);
}

list_t *list_append(list_head_t *list_head, void *data, list_t *relative)
{
  assert(list_head != NULL);
  assert(relative == NULL || _is_node_in_list(relative, list_head));
  list_t *new_node = list_alloc(data);

  _handle_list_init_empty(list_head);
  if(new_node != NULL) {
    relative == NULL ? list_add_tail(&new_node->head, &list_head->head)
                     : list_add(&new_node->head, &relative->head);
  }

  return list_head;
}

list_t *list_prepend(list_head_t *list_head, void *data, list_t *relative)
{
  assert(list_head != NULL);
  assert(relative == NULL || _is_node_in_list(relative, list_head));

  list_t *new_node = list_alloc(data);

  _handle_list_init_empty(list_head);
  if(new_node != NULL) {
    relative == NULL ? list_add(&new_node->head, &list_head->head)
                     : list_add_tail(&new_node->head, &relative->head);
  }

  return list_head;
}

void *list_node_remove(list_t *node)
{
  void *data = node->data;
  list_del(&node->head);
  free(node);
  return data;
}

list_t *list_get_next(const list_head_t *list_head, const list_t *relative)
{
  assert(list_head != NULL);
  assert(relative == NULL || _is_node_in_list(relative, list_head));

  list_t *next = NULL;
  if(!is_list_empty(list_head)) {
    if(relative) {
      next = list_first_entry(&relative->head, list_t, head);
      if(next == list_head) {
        next = NULL;
      }
    } else {
      next = list_first_entry(&list_head->head, list_t, head);
    }
  }

  return next;
}

list_t *list_get_prev(const list_head_t *list_head, const list_t *relative)
{
  assert(list_head != NULL);
  assert(relative == NULL || _is_node_in_list(relative, list_head));

  list_t *prev = NULL;

  if(!is_list_empty(list_head)) {
    if(relative) {
      prev = list_last_entry(&relative->head, list_t, head);
      if(prev == list_head) {
        prev = NULL;
      }
    } else {
      prev = list_last_entry(&list_head->head, list_t, head);
    }
  }

  return prev;
}

list_t *list_find(const list_head_t *list, const void *find, const compare_cb_t cmp)
{
  list_foreach(node, list)
  {
    if(cmp == NULL) {
      if(find == node->data) {
        return node;
      }
    } else {
      if(cmp(find, node->data) == 0) {
        return node;
      }
    }
  }

  return NULL;
}

bool is_list_empty(const list_head_t *list_head)
{
  return (list_head == NULL) || list_empty(&list_head->head) ||
         (list_head->head.prev == NULL && list_head->head.next == NULL);
}

static list_t *_handle_list_init_empty(list_t *list)
{
  if(list && (list->head.next == NULL || list->head.prev == NULL)) {
    list_init(list, list->data);
  }

  return list;
}

static bool _is_node_in_list(const list_t *find, const list_head_t *list)
{
  list_t *entry;
  if(!is_list_empty(list)) list_for_each_entry(entry, &list->head, head)
    {
      if(find == entry) {
        return true;
      }
    }

  return false;
}
