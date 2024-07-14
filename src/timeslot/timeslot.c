#include "timeslot.h"
#include <stdio.h>

void timeslot_walk(timeslot_t* timeslot)
{
  if (NULL == timeslot)
    return ;

  if (timeslot->slot + 1 < TIME_SLOT_SLOT_SIZE) {
    timeslot->slot += 1;
    return;
  }

  if (timeslot->frame + 1 < TIME_SLOT_FRAME_SIZE) {
    timeslot->frame += 1;
    timeslot->slot = 0;
    return;
  }

  if (timeslot->epoll + 1 < TIME_SLOT_EPOLL_SIZE) {
    timeslot->slot  = 0;
    timeslot->frame = 0;
    timeslot->epoll += 1;
    return;
  }

  timeslot->slot  = 0;
  timeslot->frame = 0;
  timeslot->epoll = 0;

}
