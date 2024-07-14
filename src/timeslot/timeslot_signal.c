/**
 * @file timeslot_signal.c
 * @author YToyCoder(you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "timeslot/timeslot_signal.h"
#include "timeslot/timeslot_tb.h"
#include "timeslot/timeslot.h"

/**
 * @brief timeslot tick handle
 */
uv_timer_t  timeslot_tick_req;

/**
 * @brief timeslot signal handle
 */
uv_signal_t timeslot_signal;

/**
 * @brief timeslot
 */
timeslot_t timeslot;

/**
 * @brief 
 * 
 * @param ts_tick timer handle
 */
void timeslot_tick(uv_timer_t* ts_tick);

/**
 * @brief 
 * 
 * @param signal 
 * @param sig 
 */
void on_timeslot_com(uv_signal_t* signal, int sig);

int timeslot_signal_init(uv_loop_t* timeslot_loop)
{
  if (NULL == timeslot_loop)
    return 0;

  // timeslot value init
  timeslot.epoll  = 0;
  timeslot.frame  = 0;
  timeslot.slot   = 0;

  // timer init
  uv_timer_init(timeslot_loop, &timeslot_tick_req);
  uv_timer_start(&timeslot_tick_req, timeslot_tick, 5, 5);

  // signal init
  uv_signal_init(timeslot_loop, &timeslot_signal);
  uv_signal_start(&timeslot_signal, on_timeslot_com, SIGUSR1);
  return 1;
}

void timeslot_tick(uv_timer_t*)
{
  // uv_kill(0, SIGUSR1);
  timeslot_walk(&timeslot);
}

void on_timeslot_com(uv_signal_t* signal, int sig)
{
  if (signal != &timeslot_signal)
    return;

  // tr_t tr = get_tr_in_timeslot_tb(&timeslot, 0);
  uint8_t rcvId = get_rcv_id_from_timeslot_tb(&timeslot, 0);
  // printf("tr> :%d: :%d:\n", tr.txId, tr.rxId);
  // printf("timeslot : %d-%d-%d\n", timeslot.epoll, timeslot.frame, timeslot.slot);
  // printf("rcv:: %d\n", rcvId);
}