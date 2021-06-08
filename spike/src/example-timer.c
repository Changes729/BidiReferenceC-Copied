#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "timer-task.h"

static int _continue = 1;

struct _timer_task_data
{
  timer_task_t* timer_task;
  size_t        count;
} infinity_run;

void count(void* d)
{
  struct _timer_task_data* data = d;

  printf("\rcount: %ld", data->count++);
  fflush(stdout);

  timer_task_continue(data->timer_task);
  _continue = (data->count != 10);
}

void echo_once(void* d)
{
  printf("%s\n", (const char*)d);

  infinity_run.timer_task = timer_task_new(1000, count, &infinity_run);
}

int main(void)
{
  timer_task_new(1000, echo_once, "Hello world!");

  while(_continue) {
    timer_run();
  }

  return 0;
}