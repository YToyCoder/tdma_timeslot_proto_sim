/**
 * @file timeslot_tb.h
 * @author YToyCoder(you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __timeslot_tb_head_h__
#define __timeslot_tb_head_h__

#include "util/list.h"
#include "util/uthash/uthash.h"
#include "timeslot/timeslot.h"
#include "timeslot/tr_tb.h"

#define TIME_SLOT_TB_SZ 200

/**
 * @brief timeslot table includes timeslot and tr info
 */
struct __timeslot_tb_t 
{
  timeslot_t  timeslot[TIME_SLOT_TB_SZ];
  tr_list_t   timeslot_trs[TIME_SLOT_TB_SZ];
};

typedef struct __timeslot_tb_t timeslot_tb_t;

/**
 * @brief timeslot index list
 */
struct __timeslot_idx_list_t 
{
  int idx;
  proto_list_t l;
};

typedef struct __timeslot_idx_list_t timeslot_idx_list_t;

/**
 * @brief hash table for mapping type to timeslot index list
 */
struct __timeslot_idx_kv_node_t
{
  int slot_typ;
  proto_list_t timeslot_idx;

  UT_hash_handle hh; // hash
};

typedef struct __timeslot_idx_kv_node_t timeslot_idx_map_t;

struct __timeslot_tb_init_config_t 
{
  timeslot_idx_map_t* tr_init_timeslot; // map timeslot type to timeslot index
  int num_of_frame_per_epoll;
  int num_of_slot_per_frame;
  tr_tb_t* tr_tb;// tr table
};


typedef struct __timeslot_tb_init_config_t timeslot_tb_init_config_t;

/**
 * @brief timeslot config init
 * 
 * @param init_config 
 * @return int 
 */
int timeslot_tb_config_init(timeslot_tb_init_config_t* init_config);

/**
 * @brief add timeslot index to special kind index list
 * 
 * @param init_conf 
 * @param typ 
 * @param idx 
 * @return int 
 */
int timeslot_tb_config_add(timeslot_tb_init_config_t* init_conf, int typ, int idx);

/**
 * @brief init timeslot as configured
 * 
 * @param timeslot_init_conf 
 */
void timeslot_tb_init(timeslot_tb_init_config_t* timeslot_init_conf);

/**
 * @brief 
 */
void show_timeslot_tb();

tr_t get_tr_in_timeslot_tb(timeslot_t* timeslot, uint8_t trId);

uint8_t get_rcv_id_from_timeslot_tb(timeslot_t* timeslot, uint8_t sndId);

#endif