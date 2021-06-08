#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <time.h>
#include <unistd.h>

#include "dbus-object-example/object/object-countdown.h"
#include "dbus_define.h"
#include "dbus_initial.h"
#include "dbus_root_object.h"
#include "io-flush.h"
#include "timer-task.h"

static void dbus_main_loop();
static void sigint_handler(int param);

static int      main_loop_running = 1;
DBusConnection *connection;

void print_time(void *task)
{
  time_t     timer   = time(NULL);
  struct tm *tm_info = localtime(&timer);
  size_t size = strftime(NULL, INTMAX_MAX, "%Y-%m-%d %H:%M:%S", tm_info) + 1;
  char   buffer[size];

  strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
  printf("\r%s", buffer);
  fflush(stdout);

  timer_task_continue(*(timer_task_t **)task);
}

int main(int agrc, char *argv[])
{
  timer_task_t *clock_task = NULL;

  if(!dbus_init(DBUS_BUS_SESSION,
                TEST_DBUS_BUS_NAME,
                DBUS_NAME_FLAG_REPLACE_EXISTING,
                &connection))
  {
    goto __failed;
  }

  signal(SIGINT, sigint_handler);
  register_root_object(connection);
  register_countdown_object(connection);

  clock_task = timer_task_new(1000, print_time, &clock_task);

  dbus_main_loop();

  unregister_countdown_object(connection);
  unregister_root_object(connection);
  dbus_final(connection);
  timer_task_del(clock_task);

__failed:
  return 0;
}

static void dbus_main_loop()
{
  while(main_loop_running) {
    io_flush_select(timer_next_alarm() >> 1);

    timer_run();
  }
}

static void sigint_handler(int param)
{
  printf("Receive signal int\n");


  main_loop_running = 0;
}
