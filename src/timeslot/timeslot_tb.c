/**
 * @file timeslot_tb.c
 * @author YToyCoder(you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-03
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "timeslot/timeslot_tb.h"
#include <stdio.h>

struct __timeslot_run_config_t 
{
  int slot_num_per_frame;
  int frame_num_per_epoll;
};

struct __timeslot_run_config_t run_config;

int timeslot_to_tb_idx(timeslot_t* timeslot) 
{
  return timeslot->frame;
}

timeslot_tb_t timeslot_tb;

proto_list_t* next_tr_list(proto_list_t* tr_tb_node, proto_list_t* tr_tb_head)
{
  proto_list_t* next_tr_list = tr_tb_node->next;

  if (next_tr_list == tr_tb_head)
    next_tr_list = next_tr_list->next;

  return next_tr_list;
}

void timeslot_tb_init(timeslot_tb_init_config_t* init_config)
{
  if (NULL == init_config)
    return;

  run_config.frame_num_per_epoll = init_config->num_of_frame_per_epoll;
  run_config.slot_num_per_frame = init_config->num_of_slot_per_frame;

  timeslot_t* timeslot = NULL;
  // init timeslot frame and slot
  for (
    int frame = 0; 
    frame < init_config->num_of_frame_per_epoll; 
    frame++){

    for (
      int slot = 0; 
      slot < init_config->num_of_slot_per_frame; 
      slot++) {
      int timeslot_idx = frame * init_config->num_of_slot_per_frame + slot;

      if (timeslot_idx < TIME_SLOT_TB_SZ) {
        timeslot = &timeslot_tb.timeslot[timeslot_idx];
        timeslot->epoll = 0;
        timeslot->frame = frame;
        timeslot->slot = slot;
      }
    }
  }

  proto_list_t* tr_iter = &init_config->tr_tb->tr_l_head;
  timeslot_idx_map_t *s, *tmp;
  int timeslot_cnt = 0;
  HASH_ITER(hh, init_config->tr_init_timeslot, s, tmp) {

    timeslot_idx_list_t* timeslot_idx = NULL;
    proto_list_for_each_entry(timeslot_idx, &s->timeslot_idx, l){

      if (timeslot_cnt % 2 == 0)
        tr_iter = next_tr_list(tr_iter, &init_config->tr_tb->tr_l_head);

      tr_tb_node_t* tr_tb_node = proto_list_entry(tr_iter, tr_tb_node_t, l);
      timeslot_tb.timeslot[timeslot_idx->idx].typ = s->slot_typ;
      for (int i = 0 ; i < tr_tb_node->tr_list.tr_size; i++) {
        uint8_t txId = tr_tb_node->tr_list.trs[i].txId;
        uint8_t rxId = tr_tb_node->tr_list.trs[i].rxId;
        if (timeslot_cnt % 2 == 0) 
          insert_tr(&timeslot_tb.timeslot_trs[timeslot_idx->idx], txId, rxId ) ;
        else
          insert_tr(&timeslot_tb.timeslot_trs[timeslot_idx->idx], rxId, txId ) ;
      }
      timeslot_cnt++;
    }
  }

}

int timeslot_tb_config_add(timeslot_tb_init_config_t* init_config, int typ, int idx)
{
  if (NULL == init_config)
    return 0;

  timeslot_idx_map_t* find_idx = NULL;
  HASH_FIND_INT(init_config->tr_init_timeslot, &typ, find_idx);
  if (NULL == find_idx) {
    // add idx list
    find_idx = (timeslot_idx_map_t*) malloc(sizeof(timeslot_idx_map_t));
    find_idx->slot_typ = typ;
    proto_list_init(&find_idx->timeslot_idx);
    HASH_ADD_INT(init_config->tr_init_timeslot, slot_typ, find_idx);
  }

  // add idx
  timeslot_idx_list_t* newIdx = (timeslot_idx_list_t*) malloc(sizeof(timeslot_idx_list_t));
  if (NULL == newIdx)
    return 0;

  newIdx->idx = idx;
  proto_list_insert_after(&find_idx->timeslot_idx, &newIdx->l);
  return 1;
}

int timeslot_tb_config_init(timeslot_tb_init_config_t* init_config)
{
  if (NULL == init_config)
    return 0;

  init_config->tr_init_timeslot = NULL;
  init_config->num_of_frame_per_epoll = 0;
  init_config->num_of_slot_per_frame = 0;
  return 1;
}

void __show_timeslot_tb(timeslot_tb_t* tb)
{
  if (NULL == tb)
    return;

  timeslot_t* timeslot;
  tr_list_t*  tr_list;

  for (int i = 0; i < TIME_SLOT_TB_SZ; i++) {
    timeslot = &tb->timeslot[i];
    tr_list = &tb->timeslot_trs[i];
    printf("timeslot : %05d-%02d-%02d : %02d :: ", timeslot->epoll, timeslot->frame, timeslot->slot, timeslot->typ);
    show_tr_list(tr_list);
  }
}

void show_timeslot_tb() 
{
  __show_timeslot_tb(&timeslot_tb);
}

tr_t get_tr_in_timeslot_tb(timeslot_t* timeslot, uint8_t trId)
{
  tr_t tr;
  tr.rxId = 255;
  tr.txId = 255;
  int idx = timeslot->frame * run_config.slot_num_per_frame + timeslot->slot;
  if (NULL == timeslot || idx >= TIME_SLOT_TB_SZ)
    return tr;

  tr_list_t* tr_list = &timeslot_tb.timeslot_trs[idx];
  for (int i = 0; i < tr_list->tr_size; i++) {
    if (tr_list->trs[i].rxId == trId || tr_list->trs[i].txId == trId)
      return tr_list->trs[i];
  }

  return tr;
}

uint8_t get_rcv_id_from_timeslot_tb(timeslot_t* timeslot, uint8_t sndId)
{
  uint8_t rcvId = 255;
  int idx = timeslot_to_tb_idx(timeslot);

  if (timeslot == NULL || idx >= TIME_SLOT_TB_SZ)
    return rcvId;

  tr_list_t* tr_list = &timeslot_tb.timeslot_trs[idx];
  for (int i = 0; i < tr_list->tr_size; i++) {
    if (tr_list->trs[i].txId == sndId)
      return tr_list->trs[i].rxId;
  }

  return rcvId;
}