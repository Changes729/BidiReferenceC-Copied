#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "io-flush.h"

#define USE_EPOLL 1

static bool _quit = false;

static void stdin_callback(void* user_data, fd_desc_t pkg)
{
  char buffer[256] = {};
  int  result      = read(pkg.fd, &buffer, sizeof(buffer));

  if(result > 0) {
    printf("%s\n", buffer);

    size_t len = strlen("quit");
    if(strncmp("quit", buffer, len) == 0 && !isgraph(buffer[len])) {
      _quit = true;

      io_ignore_file(0);
    }
  }
}

int main(void)
{
#if USE_EPOLL
  io_epoll_fd_init();
#endif

  io_notice_file((fd_desc_t){0, IO_NOTICE_READ}, stdin_callback, NULL);

  while(!_quit) {
#if USE_EPOLL
    io_flush_epoll(UINT32_MAX);
#else
    // io_flush_select();
    io_flush_poll(UINT32_MAX);
#endif
  }

#if USE_EPOLL
  io_epoll_fd_deinit(UINT32_MAX);
#endif

  return 0;
}