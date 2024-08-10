/**
 * @file data_channel_config.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdio.h>
#include <string.h>

#include "com/data_channel_config.h"

#define DC_CONFIG_FILE "dcconfig.json"

static
struct cJSON* gDCConfig = NULL;

#define READ_BUF_SIZE 2048

int load_config()
{
  if (NULL != gDCConfig)
  {
    return 1;
  }

  FILE* f = NULL;
  f = fopen(DC_CONFIG_FILE, "r");

  uint8_t readBuf[READ_BUF_SIZE];
  int readDataLen = 0;

  char * getRes = NULL;
  do {
    getRes = fgets(readBuf + readDataLen, READ_BUF_SIZE - readDataLen, f);

    if (getRes)
      readDataLen += strlen(readBuf + readDataLen);
  } while(getRes && readDataLen < READ_BUF_SIZE);

  gDCConfig = cJSON_Parse(readBuf);

  if (NULL == gDCConfig)
    return -1;

  return 1;
}

int get_config_local_id()
{
  if (load_config() < 0) 
    return 0;

  cJSON * localId = cJSON_GetObjectItem(gDCConfig, "local_id");

  if (NULL == localId)
    return 0;

  if (cJSON_IsNumber(localId))
    return localId->valueint;

  return 0;
}

int get_channel_config(channel_config_t* config)
{
  if (NULL == config)
    return 0;

  if (load_config() < 0) 
    return 0;

  const cJSON *const channels = cJSON_GetObjectItem(gDCConfig, "channels");
  if(!cJSON_IsArray(channels)) 
    return 0;

  cJSON * channelConfig = NULL;
  int channelCnt = 0;
  int configGet = 0;

  cJSON_ArrayForEach(channelConfig, channels)
  {
    if (channelCnt != 0)
      break;
    
    if (channelCnt == 0)
    {
      const cJSON *const channelId = cJSON_GetObjectItem(channelConfig, "id");

      if (cJSON_IsNumber(channelId))
      {
        config->channel_id = channelId->valueint;
      }

      const cJSON *const channelName = cJSON_GetObjectItem(channelConfig, "name");
      if (cJSON_IsString(channelName))
      {
        strcpy(config->channel_name, channelName->valuestring);
      }

      configGet = cJSON_IsString(channelName) | cJSON_IsNumber(channelId);
    }

    channelCnt++;
  }

  return configGet;
}
