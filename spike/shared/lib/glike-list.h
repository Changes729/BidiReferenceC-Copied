/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _G_LIKE_LIST_H
#define _G_LIKE_LIST_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <stdlib.h>

#include "list-define.h"
#include "typedef.h"

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef struct list
{
  void *           data;
  struct list_head head;
} list_t;
typedef list_t list_head_t;

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/** Init -----------------------------------------------------------*/

/**
 * always be success. the retval is the input list-head. if list is NULL, nothing todo.
 */
list_head_t *list_init(list_head_t *list, void *data);

/**
 * if retval not NULL, means success.
 */
list_head_t *list_alloc(void *data);

/**
 * always be success. if list is NULL, nothing todo.
 */
void list_free(list_head_t *list);

/** Add ------------------------------------------------------------*/
/**
 * @note DEBUG will assert the input list_head NOT NULL.
 *             and the relative should be IN LIST.
 *
 * @param relative if relative is NULL, new node will be added at tail.
 *
 * @retval if not NULL, means success.
 */
list_t *list_append(list_head_t *list_head, void *data, list_t *relative);

/**
 * @note DEBUG will assert the input list_head NOT NULL.
 *             and the relative should be IN LIST.
 *
 * @param relative if relative is NULL, new node will be added at ahead.
 *
 * @retval if not NULL, means success.
 */
list_t *list_prepend(list_head_t *list_head, void *data, list_t *relative);

/** Del ------------------------------------------------------------*/
/**
 * remove one node from list. retval is the data.
 */
void *list_node_remove(list_t *node);

/** Update ---------------------------------------------------------*/
/** Search ---------------------------------------------------------*/
/**
 * @param relative if NULL, relative will be the list_head.
 *
 * @retval return the next node after the relative. this may be a NULL.
 */
list_t *list_get_next(const list_head_t *list_head, const list_t *relative);

/**
 * @param relative if NULL, relative will be the list_head.
 *
 * @retval return the previous node before the relative. this may be a NULL.
 */
list_t *list_get_prev(const list_head_t *list_head, const list_t *relative);

/**
 * retval is the first node what U want. this may be a NULL.
 */
list_t *list_find(const list_head_t *list, const void *find, const compare_cb_t by);

/** Helper ---------------------------------------------------------*/
bool is_list_empty(const list_head_t *list);

#define list_foreach(val, list)                                                \
  for(list_t *val = list_get_next(list, NULL); val != NULL;                    \
      val         = list_get_next(list, val))

#define list_foreach_r(val, list)                                              \
  for(list_t *val = list_get_prev(list, NULL); val != NULL;                    \
      val         = list_get_prev(list, val))

#define list_foreach_data(_val, list)                                          \
  for(int _run = 1; _run == 1; _run = 0)                                       \
    for(list_t *val = list_get_next(list, NULL); val != NULL;                  \
        val         = list_get_next(list, val))                                \
      for((_val) = val->data, _run = 1; _run == 1; _run = 0)

#define list_foreach_data_r(_val, list)                                        \
  for(int _run = 1; _run == 1; _run = 0)                                       \
    for(list_t *val = list_get_prev(list, NULL); val != NULL;                  \
        val         = list_get_prev(list, val))                                \
      for((_val) = val->data, _run = 1; _run == 1; _run = 0)

#define list_add_data_head(list_head, data) list_append(list_head, data, NULL)
#define list_add_data_tail(list_head, data) list_prepend(list_head, data, NULL)
#define list_get_first(list_head)           list_get_next(list_head, NULL)
#define list_get_last(list_head)            list_get_prev(list_head, NULL)

#define list_cleanup_foreach_data(_val, list)                                  \
  for(int _run = 1; _run == 1; _run = 0)                                       \
    for(list_t *_free = list_get_next(list, NULL); _free != NULL;              \
        _free         = list_get_next(list, NULL))                             \
      for(_val = _free->data, _run = 1; _run == 1; _run = 0)

#define list_cleanup(list)                                                     \
  do {                                                                         \
    void *_UNUSED_ __attribute__((unused));                                    \
    list_cleanup_foreach_data(_UNUSED_, list);                                 \
  } while(0)

#ifdef __cplusplus
}
#endif
#endif /* _G_LIKE_LIST_H */
