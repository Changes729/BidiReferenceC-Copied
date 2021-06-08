/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SET_LIST_H
#define _SET_LIST_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include "glike-list.h"

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef operation_cb_t cleanup_cb_t;

typedef struct _set_list
{
  list_head_t list_head;

  unsigned          count;
  CompareCallback_t cb_cmp;
  cleanup_cb_t      cb_cleanup;
} sets_t;

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
sets_t* sets_alloc(CompareCallback_t cmp, cleanup_cb_t cleanup);
sets_t* sets_init(sets_t* set, CompareCallback_t, cleanup_cb_t);
void    sets_free(sets_t*);

sets_t* sets_add(sets_t* set, void* data);
void    sets_remove(sets_t* set, void* data);
void    sets_cleanup(sets_t* set);

bool         sets_find(const sets_t* set, const void* data, void** out);
unsigned     sets_length(const sets_t* set);
void         sets_foreach(sets_t* set, Function_t callback, void* userdata);
list_head_t* sets_get_listhead(sets_t* sets);

#define is_sets_empty(_sets) sets_length(_sets) == 0
#define sets_foreach_data(_val, _sets)                                            \
  for(list_head_t* _head = sets_get_listhead(_sets); _head != NULL; _head = NULL) \
  list_foreach_data(_val, _head)

#ifdef __cplusplus
}
#endif
#endif /* _SET_LIST_H */
