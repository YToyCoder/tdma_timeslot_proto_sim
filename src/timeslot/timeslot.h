/**
 * @file timeslot.h
 * @author YToyCoder(you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __timeslot_c_head_h__
#define __timeslot_c_head_h__

#include <stdint.h>

/**
 * @brief timeslot info
 */
struct __timeslot_t {
  uint32_t  epoll;
  uint8_t   frame;
  uint8_t   slot; 

  uint32_t  typ; // timeslot type
};

typedef struct __timeslot_t timeslot_t;

void timeslot_walk(timeslot_t* timeslot);

#define TIME_SLOT_SLOT_SIZE 20
#define TIME_SLOT_FRAME_SIZE 10
#define TIME_SLOT_EPOLL_SIZE (24 * 60 * 60)

#endif