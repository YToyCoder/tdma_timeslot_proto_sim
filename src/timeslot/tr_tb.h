/**
 * @file tr_tb.h
 * @author YToyCoder(you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __tr_tb_head_h__
#define __tr_tb_head_h__

#include <stdint.h>
#include "util/list.h"

/**
 * @brief tx and tx id 
 */
struct __tr_t 
{
  uint8_t txId;
  uint8_t rxId;
};

typedef struct __tr_t tr_t;

#define MAX_TR_SZ 8

/**
 * @brief list of tr
 */
struct __tr_list_t 
{
  int   tr_size;
  tr_t  trs[MAX_TR_SZ];
};

typedef struct __tr_list_t tr_list_t;

/**
 * @brief init tr list
 * 
 * @param tr_list 
 * @return int 
 */
int tr_list_init(tr_list_t* tr_list);

int insert_tr(tr_list_t* tr_list, uint8_t txId, uint8_t rxId);

struct __tr_tb_node_t
{
  tr_list_t     tr_list;
  proto_list_t  l; // link list
};

typedef struct __tr_tb_node_t tr_tb_node_t;

struct __tr_tb_t 
{
  proto_list_t  tr_l_head;
  int           tb_size;
};

typedef struct __tr_tb_t tr_tb_t;

/**
 * @brief init tx and tx table
 * 
 * @param tr_tb 
 */
void tr_tb_init(tr_tb_t* tr_tb);

void tr_tb_clear(tr_tb_t* tr_tb);

/**
 * @brief generate tr table for netScale
 * 
 * @param netScale 
 * @param tr_tb 
 * @return int 
 */
int generate_tr_tb(int netScale, tr_tb_t* tr_tb);

void tr_tb_insert_list(tr_tb_t* tb, tr_list_t* tr_list);

/**
 * @brief 
 * 
 * @param tr_list 
 */
void show_tr_list(tr_list_t* tr_list);

/**
 * @brief 
 * 
 * @param tr_tb 
 */
void show_tr_tb(tr_tb_t* tr_tb);

/**
 * @brief 
 * 
 * @param tr_list 
 * @param txId 
 * @param rxId 
 * @return int 
 */
int insert_tr(tr_list_t* tr_list, uint8_t txId, uint8_t rxId);

#endif