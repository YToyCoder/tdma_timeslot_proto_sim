/**
 * @file com_channel.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __com_channel_head_h__
#define __com_channel_head_h__

#include <stdint.h>

void com_init();

enum e_com_resourcer_t 
{
  e_com_resourcer_t_omni,
  e_com_resourcer_t_data,
  e_com_resourcer_t_end
};

struct _com_resource_cfg_t 
{
  char* name;
  int   id;
};

#endif