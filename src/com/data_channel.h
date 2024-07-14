/**
 * @file data_channel.h
 * @author YToyCoder(you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __data_channel_head_h__
#define __data_channel_head_h__

#include <uv.h>

typedef void(*on_channel_read)(uint8_t* buf, ssize_t nread);

struct __dchannel_t {
  uv_udp_t snd_sock;
  uv_udp_t rcv_sock;

  struct sockaddr_in local_addr;
  struct sockaddr_in broadcast_addr;
  int   local_port;
  int   broadcast_port;
  on_channel_read onread;
};

typedef struct __dchannel_t data_channel_t;

int data_channel_init(data_channel_t* data_channel,
  const char* local_addr, int local_port, 
  const char* broard_cast_addr, int broadcast_port);

int data_channel_start(uv_loop_t* loop,data_channel_t*, on_channel_read channel_read);

int data_channel_test(uv_loop_t* loop);

#endif