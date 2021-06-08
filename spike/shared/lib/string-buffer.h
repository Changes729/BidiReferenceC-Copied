/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _STRING_BUFFER_H
#define _STRING_BUFFER_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <string.h>

/* Public namespace ----------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* Public define -------------------------------------------------------------*/
typedef struct string string_t;

/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
string_t *string_new(const char *str);
void      string_delete(string_t *string);
int       string_append_printf(string_t *, const char *format, ...);

const char *string_c_str(string_t *string);

#ifdef __cplusplus
}
#endif
#endif /* _STRING_BUFFER_H */
