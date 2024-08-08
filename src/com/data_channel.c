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

#define DATA_CHANNEL_SIZE (1 + 4) // id(1Byte) + Len(4Byte)

static void alloc_buffer(uv_handle_t* hd, size_t suggested_size, uv_buf_t* buf) 
{
  if (buf == NULL) {
    return;
  }

  *buf = uv_buf_init(malloc(suggested_size), suggested_size);
}

static void write_head(data_channel_t* channel, uint8_t* buffer, uint32_t buffer_len, uint32_t data_len)
{
  buffer[0] = channel->local_id;
  buffer[1] = (data_len >> 24) & 0xff;
  buffer[2] = (data_len >> 16) & 0xff;
  buffer[3] = (data_len >>  8) & 0xff;
  buffer[4] =  data_len & 0xff;
}

int build_channel_data(data_channel_t* channel, uint8_t* buffer, uint32_t buffer_len, uint8_t* data, uint32_t data_len)
{
  if (NULL == channel)
    return -1;

  if (NULL == buffer || buffer_len == 0)
  {
    return -1;
  }

  if (DATA_CHANNEL_SIZE + data_len > buffer_len)
  {
    return -1;
  }

  write_head(channel, buffer, buffer_len, data_len);

  memcpy(buffer + DATA_CHANNEL_SIZE, data, data_len);

  return data_len + DATA_CHANNEL_SIZE;
}

static void on_read(uv_udp_t* hd, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags) 
{

  if (nread < 0) {
    fprintf(stderr, "Read error %s\n", uv_err_name(nread));
    // uv_close((uv_handle_t*) hd, NULL);
    free(buf->base);
    return;
  }

  data_channel_t* data_channel = proto_container_of(hd, data_channel_t, rcv_sock);
  int rcv_id = buf->base[0];
  if (data_channel->local_id == rcv_id)
  {
    free(buf->base);
    return;
  }

  if (NULL == data_channel->onread) {
    free(buf->base);
    return;
  }

  data_channel->onread(buf->base, nread);
}

int data_channel_init(data_channel_t* data_channel,
  const char* local_addr, int local_port, 
  const char* broard_cast_addr, int broadcast_port, int local_id)
{
  if (data_channel == NULL)
    return -1;
  uv_ip4_addr(local_addr, local_port, &data_channel->local_addr);
  data_channel->local_port = local_port;
  uv_ip4_addr(broard_cast_addr, local_port, &data_channel->broadcast_addr);
  data_channel->broadcast_port = broadcast_port;
  data_channel->onread = NULL;
  data_channel->local_id = local_id;
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
#ifdef WIN32
    (const struct sockaddr*) &data_channel->local_addr, 
#elif defined __linux__
    (const struct sockaddr*) & data_channel->broadcast_addr, 
#else 
#error "not support platform"
#endif
    UV_UDP_REUSEADDR);

  char local_ip4_addr[20] = {0}; 
  uv_ip4_name((const struct sockaddr_in*) &data_channel->local_addr, local_ip4_addr, 20);
  char broadcast_ip4_addr[20] = {0};
  uv_ip4_name((const struct sockaddr_in*) &data_channel->broadcast_addr, broadcast_ip4_addr, 20);

  if (uv_err < 0) {
    fprintf(stderr, 
      "bind rcv udp error, error msg : %s local addr %s, remote addr %s\n", 
      uv_strerror(uv_err), 
      local_ip4_addr, 
      broadcast_ip4_addr);
    return -1;
  }

  data_channel->onread = channel_read;

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

#define MSG_C "Broad Msg"

static uv_buf_t discover_msg;
dc_snd_t _dc_snd;
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

  // uv_udp_send(
  //   snd_req, 
  //   &dc_snd->dc->snd_sock, 
  //   &discover_msg, 1, 
  //   (const struct sockaddr *) &dc_snd->dc->broadcast_addr, on_snd);

  char* msg = MSG_C;
  data_channel_snd_data(dc_snd->dc, msg, sizeof(MSG_C));

}

static uint8_t arr_buf[1024];

data_channel_t data_channel;
int data_channel_test(uv_loop_t* loop)
{
  memset(arr_buf, 0, 1024);
  discover_msg = uv_buf_init(arr_buf, 1024);
  char* msg = MSG_C;
  _dc_snd.dc = &data_channel;
  memcpy(discover_msg.base, msg, sizeof(MSG_C));
  data_channel_init(&data_channel, "192.168.1.4", 1098, "224.1.2.244", 98, 0);
  data_channel_start(loop, &data_channel, on_read_user);
  // uv_udp_send(&_dc_snd.req, &data_channel.snd_sock, &discover_msg, 1,
  //   (const struct sockaddr *) &_dc_snd.dc->broadcast_addr, on_snd);
  data_channel_snd_data(&data_channel, msg, sizeof(MSG_C));
}


uv_buf_t snd_buf;
int data_channel_snd_data(data_channel_t* channel, uint8_t* data, uint32_t data_len)
{
  int required_len = data_len + DATA_CHANNEL_SIZE;
  uint8_t* snd_buffer = malloc(required_len);

  if (build_channel_data(channel, snd_buffer, required_len, data, data_len) < 0)
  {
    free(snd_buffer);
    return -1;
  }

  snd_buf = uv_buf_init(snd_buffer, required_len);
  uv_udp_send(&_dc_snd.req, &channel->snd_sock, &snd_buf, 1,
    (const struct sockaddr *) &_dc_snd.dc->broadcast_addr, on_snd);

}