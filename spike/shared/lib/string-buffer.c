/** See a brief introduction (right-hand button) */
#include "string-buffer.h"
/* Private include -----------------------------------------------------------*/
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
struct string
{
  char * buffer;
  size_t length;
  size_t buffer_size;
};

/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
struct string *string_new(const char *str)
{
  struct string *string = malloc(sizeof(struct string));

  if(NULL != str) {
    string->length      = strlen(str);
    string->buffer_size = (string->length << 1);
  } else  // str == NULL
  {
    string->length      = 0;
    string->buffer_size = 16;
    str                 = "";
  }

  string->buffer = malloc(string->buffer_size);
  strcpy(string->buffer, str);

  return string;
}

void string_delete(string_t *string)
{
  if(string == NULL) return;

  free(string->buffer);
  free(string);
}

int string_append_printf(struct string *string, const char *format, ...)
{
  va_list args, copy;
  va_start(args, format);
  va_copy(copy, args);

  size_t length = vsnprintf(NULL, 0, format, args) + 1;
  char   buffer[length];
  vsnprintf(buffer, length, format, copy);

  va_end(copy);
  va_end(args);

  if(string->length + length >= string->buffer_size) {
    if(string->buffer_size < (SIZE_MAX >> 1)) {
      string->buffer_size <<= 1;
    } else {
      string->buffer_size = SIZE_MAX;
    }

    char *tmpbuf = realloc(string->buffer, string->buffer_size);
    if(tmpbuf != NULL) {
      string->buffer = tmpbuf;
    } else {
      length = -1;
      goto __failed;
    }
  }

  length = snprintf(&string->buffer[string->length],
                    string->buffer_size - string->length,
                    "%s",
                    buffer);
  string->length += length;

__failed:
  return length;
}

const char *string_c_str(string_t *string)
{
  return (string == NULL || string->buffer == NULL) ? "" : string->buffer;
}
