/**
 * @file data_channel_config.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __data_channel_config_head_h__
#define __data_channel_config_head_h__

#include <stdint.h>
#include "util/cJSON/cJSON.h"

#define IP_NAME_BUF_LEN 40

typedef struct __channel_config 
{
  uint8_t channel_id;
  char    channel_name[IP_NAME_BUF_LEN];
} channel_config_t;

int get_config_local_id();
int get_channel_config(channel_config_t* config);

#endif // __data_channel_config_head_h__