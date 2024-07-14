#include <uv.h>
#include <stdlib.h>
#include <stdio.h>

#include "timeslot/timeslot_signal.h"
#include "timeslot/timeslot_tb.h"
#include "timeslot/tr_tb.h"
#include "com/data_channel.h"

int main(int argc, char const *argv[])
{
  timeslot_tb_init_config_t init_config;

  timeslot_tb_config_init(&init_config);
  init_config.num_of_frame_per_epoll = 10;
  init_config.num_of_slot_per_frame = 20;

  for (int i = 0; i < 10; i++)
  {
    timeslot_tb_config_add(&init_config, 1, i);
  }

  printf("--------------------------------proto sim ------------------------\n");
  printf("init tr table...\n");
  tr_tb_t tr_tb;
  tr_tb_init(&tr_tb);
  generate_tr_tb(4, &tr_tb);
  // show_tr_tb(&tr_tb);

  init_config.tr_tb = &tr_tb;
  printf("init timeslot table\n");
  timeslot_tb_init(&init_config);
  show_timeslot_tb();

  // uv loop init
  uv_loop_t* loop = malloc(sizeof(uv_loop_t));
  uv_loop_init(loop);

  printf("..init \n");
  timeslot_signal_init(loop);
  // udp_channel_init(loop);
  data_channel_test(loop);
  return uv_run(loop, UV_RUN_DEFAULT);
}
