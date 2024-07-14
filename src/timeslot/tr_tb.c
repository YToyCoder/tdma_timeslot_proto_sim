/**
 * @file tr_tb.c
 * @author YToyCoder(you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "timeslot/tr_tb.h"
#include "util/uthash/utarray.h"

int tr_list_init(tr_list_t* tr_list)
{
  if (NULL == tr_list)
    return 0;

  tr_list->tr_size = 0;
  memset(tr_list->trs, 0, sizeof(MAX_TR_SZ));
  return 1;
}

int insert_tr(tr_list_t* tr_list, uint8_t txId, uint8_t rxId)
{
  if (NULL == tr_list)
    return 0;

  if (tr_list->tr_size >= MAX_TR_SZ)
    return 0;

  tr_list->trs[tr_list->tr_size].rxId = rxId;
  tr_list->trs[tr_list->tr_size].txId = txId;
  tr_list->tr_size++;
  return 1;
}

void tr_tb_clear(tr_tb_t* tr_tb)
{
  if (NULL == tr_tb)
    return;

  while(!proto_list_isempty(&tr_tb->tr_l_head)) {
    proto_list_t* next = tr_tb->tr_l_head.next;
    proto_list_remove(next);
    tr_tb_node_t* tr_tb_node = proto_list_entry(next, tr_tb_node_t, l);
    free(tr_tb_node);
  }
}

typedef UT_array mmb_arr_t;

void __generate_serial_mmb(mmb_arr_t* mmb_arr, int generate_scale)
{
  if (generate_scale == 0 || generate_scale % 2 == 1)
    return ;

  utarray_init(mmb_arr, &ut_int_icd);
  for (int i = 0; i < generate_scale; i++) {
    utarray_push_back(mmb_arr, &i);
  }
}

#define USED_MARK_SZ 16

#define __Forward(i, m)  (i + 1 == m ? 0 : i + 1)
#define __Backward(i, m) (i - 1 < 0 ? m -1 : i -1)

int mmb_arr_idx_forward(int idx, int mmb_sz, uint8_t used_mark_tb[USED_MARK_SZ])
{
  int next = __Forward(idx, mmb_sz); 
  while(used_mark_tb[next] != 0) {
    next = __Forward(next, mmb_sz);
  }
  used_mark_tb[next] = 1;
  return next;
}

int mmb_arr_idx_backward(int idx, int mmb_sz, uint8_t used_mark_tb[USED_MARK_SZ])
{
  int next = __Backward(idx, mmb_sz);
  while(used_mark_tb[next] != 0) {
    next = __Backward(next, mmb_sz); 
  }
  used_mark_tb[next] = 1;
  return next;
}

#define utarray_int_elt(a, j) (*(int*) utarray_eltptr(a,j))

void __tr_tb_generate_list(
  tr_tb_node_t* tr_list, 
  mmb_arr_t* mmb_arr, 
  uint8_t used_mark_tb[USED_MARK_SZ], 
  int start_tx_idx, 
  int start_rx_idx)
{
  memset(used_mark_tb, 0, USED_MARK_SZ);
  insert_tr(&tr_list->tr_list, utarray_int_elt(mmb_arr, start_tx_idx),  utarray_int_elt(mmb_arr, start_rx_idx));
  used_mark_tb[start_rx_idx] = 1;
  used_mark_tb[start_tx_idx] = 1;
  int mmb_sz = utarray_len(mmb_arr);
  int tx_idx = mmb_arr_idx_forward(start_rx_idx, mmb_sz, used_mark_tb);
  int rx_idx = mmb_arr_idx_backward(start_rx_idx, mmb_sz, used_mark_tb);
  insert_tr(&tr_list->tr_list, utarray_int_elt(mmb_arr, tx_idx),  utarray_int_elt(mmb_arr, rx_idx));
  for (int i = 0; i < (mmb_sz / 2) - 2; i++) {
    tx_idx = mmb_arr_idx_forward(tx_idx, mmb_sz, used_mark_tb);
    rx_idx = mmb_arr_idx_backward(rx_idx, mmb_sz, used_mark_tb);
    insert_tr(&tr_list->tr_list, utarray_int_elt(mmb_arr, tx_idx),  utarray_int_elt(mmb_arr, rx_idx));
  }
}

void tr_tb_generate_in_mmb(tr_tb_t* tr_tb, mmb_arr_t* mmb_arr)
{
  if (NULL == tr_tb || NULL == mmb_arr)
    return;

  uint8_t used_mark_tb[USED_MARK_SZ];
  memset(used_mark_tb, 0, USED_MARK_SZ);

  int mmb_arr_len = utarray_len(mmb_arr);
  for (int i = 0; i < mmb_arr_len - 1; i++) {
    tr_tb_node_t* tr_list = (tr_tb_node_t*) malloc(sizeof(tr_tb_node_t));
    __tr_tb_generate_list(tr_list, mmb_arr, used_mark_tb, 0, i + 1);
    proto_list_insert_after(&tr_tb->tr_l_head, &tr_list->l);
  }
}

void __tr_tb_generate(int generate_scale, tr_tb_t* tr_tb)
{
  mmb_arr_t mmb_arr;
  __generate_serial_mmb(&mmb_arr, generate_scale);
  tr_tb_generate_in_mmb(tr_tb, &mmb_arr);
}

void tr_tb_insert_list(tr_tb_t* tb, tr_list_t* tr_list)
{
  if (NULL == tb || NULL == tr_list)
    return;
}

int generate_tr_tb(int netScale, tr_tb_t* tr_tb)
{
  if (NULL == tr_tb || netScale <= 0)
    return 0;

  tr_tb_clear(tr_tb);

  int generate_scale = netScale + netScale % 2;
  if (generate_scale == 2) {
    tr_tb_node_t* tr_tb_node = (tr_tb_node_t*) malloc(sizeof(tr_tb_node_t));
    tr_list_init(&tr_tb_node->tr_list);
    insert_tr(&tr_tb_node->tr_list, 0, 1);
    proto_list_insert_after(&tr_tb->tr_l_head, & tr_tb_node->l);
    return 1;
  }

  __tr_tb_generate(generate_scale, tr_tb);
  return 1;
}

void show_tr_list(tr_list_t* tr_list)
{
  printf(" >: ");
  if (NULL == tr_list) {
    printf("\n");
    return;
  }

  for (int i = 0 ; i < tr_list->tr_size; i++) {
    tr_t* tr = &tr_list->trs[i];
    printf("(%2d-%2d) ", tr->txId, tr->rxId);
  }
  printf("\n");

}

void show_tr_tb(tr_tb_t* tr_tb)
{
  if (NULL == tr_tb)
    return;

  tr_tb_node_t* tr_tb_node = NULL;
  proto_list_for_each_entry(tr_tb_node, &tr_tb->tr_l_head, l) {
    if (NULL == tr_tb_node)
      continue;

    show_tr_list(&tr_tb_node->tr_list);
  }
}

void tr_tb_init(tr_tb_t* tr_tb)
{
  if (NULL == tr_tb)
    return;

  tr_tb->tb_size = 0;
  proto_list_init(&tr_tb->tr_l_head);
}