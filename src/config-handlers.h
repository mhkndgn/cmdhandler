#ifndef CONFIG_HANDLERS_H
#define CONFIG_HANDLERS_H

#include "defines.h"

extern int add_command(char *filename, char *value_ptr, char field_arr[MAX_CMD_FIELD_NUM][MAX_CMD_FIELD_LEN], int field_num);
extern int cfg_key_kafka_host(char *filename, char *value_ptr, char field_arr[MAX_CMD_FIELD_NUM][MAX_CMD_FIELD_LEN], int field_num);
extern int cfg_key_kafka_topic(char *filename, char *value_ptr, char field_arr[MAX_CMD_FIELD_NUM][MAX_CMD_FIELD_LEN], int field_num);
extern int cfg_key_cpe_uuid(char *filename, char *value_ptr, char field_arr[MAX_CMD_FIELD_NUM][MAX_CMD_FIELD_LEN], int field_num);
extern int cfg_key_time_interval(char *filename, char *value_ptr, char field_arr[MAX_CMD_FIELD_NUM][MAX_CMD_FIELD_LEN], int field_num);

#endif
