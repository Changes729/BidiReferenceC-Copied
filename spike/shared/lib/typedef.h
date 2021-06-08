
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _TYPEDEF_H
#define _TYPEDEF_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef int (*compare_cb_t)(const void* a, const void* b);
typedef void (*operation_cb_t)(void* p);

typedef int (*Function_t)(void* data, void* user_data);
typedef void (*DestroyCallback_t)(void* data);
typedef int (*CompareCallback_t)(const void* a, const void* b);
typedef void (*CompareCallbackFull_t)(const void* a,
                                      const void* b,
                                      void*       user_data);
typedef void* (*CopyCallback_t)(const void* src, void* data);

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/

#ifdef __cplusplus
}
#endif
#endif /* _TYPEDEF_H */
