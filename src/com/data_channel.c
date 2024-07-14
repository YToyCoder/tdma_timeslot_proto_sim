/**
 * @file data_channel.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "com/data_channel.h"
#include "util/list.h"
#include <string.h>
#include <stdlib.h>

static void alloc_buffer(uv_handle_t* hd, size_t suggested_size, uv_buf_t* buf) 
{
  if (buf == NULL) {
    return;
  }

  *buf = uv_buf_init(malloc(suggested_size), suggested_size);
}


static void on_read(uv_udp_t* hd, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags) 
{
  printf("read??\n");
  if (nread < 0) {
    fprintf(stderr, "Read error %s\n", uv_err_name(nread));
    // uv_close((uv_handle_t*) hd, NULL);
    free(buf->base);
    return;
  }
  data_channel_t* data_channel = proto_container_of(hd, data_channel_t, rcv_sock);

  if (NULL == data_channel->onread) {
    free(buf->base);
    return;
  }

  data_channel->onread(buf->base, nread);
}

int data_channel_init(data_channel_t* data_channel,
  const char* local_addr, int local_port, 
  const char* broard_cast_addr, int broadcast_port)
{
  if (data_channel == NULL)
    return -1;
  uv_ip4_addr(local_addr, local_port, &data_channel->local_addr);
  data_channel->local_port = local_port;
  uv_ip4_addr(broard_cast_addr, local_port, &data_channel->broadcast_addr);
  data_channel->broadcast_port = broadcast_port;
  data_channel->onread = NULL;
  return 0;
}

struct sockaddr_in any_sockaddr;
int data_channel_start(uv_loop_t* loop,data_channel_t* data_channel, on_channel_read channel_read)
{
  // init rcv socket
  int uv_err = uv_udp_init(loop, &data_channel->rcv_sock);
  if (uv_err < 0) {
    fprintf(stderr, "init rcv udp error, error msg : %s\n", uv_strerror(uv_err));
    return -1;
  }

  uv_err = uv_udp_bind(
    &data_channel->rcv_sock, 
    // (const struct sockaddr*) &data_channel->local_addr, 
    (const struct sockaddr*) & data_channel->broadcast_addr, 
    UV_UDP_REUSEADDR);

  if (uv_err < 0) {
    fprintf(stderr, "bind rcv udp error, error msg : %s\n", uv_strerror(uv_err));
    return -1;
  }

  data_channel->onread = channel_read;

  char local_ip4_addr[20] = {0}; 
  uv_ip4_name((const struct sockaddr_in*) &data_channel->local_addr, local_ip4_addr, 20);
  char broadcast_ip4_addr[20] = {0};
  uv_ip4_name((const struct sockaddr_in*) &data_channel->broadcast_addr, broadcast_ip4_addr, 20);
  uv_err = uv_udp_set_membership(&data_channel->rcv_sock, broadcast_ip4_addr, local_ip4_addr, UV_JOIN_GROUP);
  uv_udp_recv_start(&data_channel->rcv_sock, alloc_buffer, on_read);
  /// 

  uv_udp_init(loop, &data_channel->snd_sock);
  uv_udp_bind(&data_channel->snd_sock, (const struct sockaddr*) &data_channel->local_addr, 0);
  uv_udp_set_broadcast(&data_channel->snd_sock, 1);
}

void on_read_user(uint8_t* buf, ssize_t buf_len)
{
  printf("rcv data %ld\n", buf_len);
}

struct __dc_snd_t {
  uv_udp_send_t req;
  data_channel_t* dc;
};

typedef struct __dc_snd_t dc_snd_t;

static uv_buf_t discover_msg;
static int g_snd_cnt = 0;
void on_snd(uv_udp_send_t* snd_req, int status) {
  if (g_snd_cnt >= 100){
    return;
  }
  g_snd_cnt++;
  // printf("send data\n");
  if (status) {
      printf("Send error %s\n", uv_strerror(status));
      return;
  }

  dc_snd_t* dc_snd = (dc_snd_t*) snd_req;

  uv_udp_send(
    snd_req, 
    &dc_snd->dc->snd_sock, 
    &discover_msg, 1, 
    (const struct sockaddr *) &dc_snd->dc->broadcast_addr, on_snd);

}

#define MSG_C "Broad Msg"

static uint8_t arr_buf[1024];

dc_snd_t _dc_snd;
data_channel_t data_channel;
int data_channel_test(uv_loop_t* loop)
{
  memset(arr_buf, 0, 1024);
  discover_msg = uv_buf_init(arr_buf, 1024);
  char* msg = MSG_C;
  _dc_snd.dc = &data_channel;
  memcpy(discover_msg.base, msg, sizeof(MSG_C));
  data_channel_init(&data_channel, "192.168.1.3", 1098, "224.0.1.244", 1086);
  data_channel_start(loop, &data_channel, on_read_user);
  uv_udp_send(&_dc_snd.req, &data_channel.snd_sock, &discover_msg, 1,
    (const struct sockaddr *) &_dc_snd.dc->broadcast_addr, on_snd);
}