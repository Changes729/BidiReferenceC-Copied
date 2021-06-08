/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _IO_FLUSH_H
#define _IO_FLUSH_H
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
enum io_notice_type {
  IO_NOTICE_READ  = 1 << 0,
  IO_NOTICE_WRITE = 1 << 1,
  IO_NOTICE_ERR   = 1 << 2,
  IO_NOTICE_HUP   = 1 << 3,
};

/* Public template -----------------------------------------------------------*/
typedef struct
{
  int   fd;
  short flag;
} fd_desc_t;

typedef void (*fd_callback_t)(void* user_data, fd_desc_t pkg);

/* Public function prototypes ------------------------------------------------*/
/** select, poll ---------------------------------------------------*/
bool io_epoll_fd_init();
void io_epoll_fd_deinit();

void io_flush_select(uint32_t ms);
void io_flush_poll(uint32_t ms);
void io_flush_epoll(uint32_t ms);

bool io_notice_file(fd_desc_t pkg, fd_callback_t callback, void* user_data);
void io_ignore_file(int fd);

bool io_notice_file_update(fd_desc_t pkg);

#ifdef __cplusplus
}
#endif
#endif /* _IO_FOCUS_H */
