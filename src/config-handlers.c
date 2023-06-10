#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config-handlers.h"
#include "globals.h"

int add_command(char *filename, char *value_ptr, char field_arr[MAX_CMD_FIELD_NUM][MAX_CMD_FIELD_LEN], int field_num)
{
	if(!strcmp(value_ptr, "") ||(field_num<1)){
		fprintf(stderr,"command cannot be added!\n");
		return ERR;
	}
	else{
		for(int i=0; i<MAX_CMD_FIELD_NUM; i++){
			strncpy(commands[cmd_num].cmd_fields[i],field_arr[i],MAX_CMD_FIELD_LEN);
		}
		commands[cmd_num].cmd = value_ptr;
		commands[cmd_num].cmd_fields_num=field_num;
		cmd_num++;
		return SUCCESS;
	}
}

int cfg_key_kafka_host(char *filename, char *value_ptr, char field_arr[MAX_CMD_FIELD_NUM][MAX_CMD_FIELD_LEN], int field_num)
{
	if(!strcmp(value_ptr, "")){
		fprintf(stderr,"kafka_host cannot be added!using default: localhost:9092\n");
		config.kafka_host = "localhost:9092";
		return ERR;
	}
	else{
		config.kafka_host = value_ptr;
		return SUCCESS;
	}
} 
int cfg_key_kafka_topic(char *filename, char *value_ptr, char field_arr[MAX_CMD_FIELD_NUM][MAX_CMD_FIELD_LEN], int field_num)
{
	if(!strcmp(value_ptr, "")){
		fprintf(stderr,"kafka_topic cannot be added!using default: default-topic\n");
		config.kafka_topic = "default-topic";
		return ERR;
	}
	else{
		config.kafka_topic = value_ptr;
		return SUCCESS;
	}
}

int cfg_key_cpe_uuid(char *filename, char *value_ptr, char field_arr[MAX_CMD_FIELD_NUM][MAX_CMD_FIELD_LEN], int field_num)
{
	if(!strcmp(value_ptr, "")){
		fprintf(stderr,"cpe_label cannot be added!\n");
		return ERR;
	}
	else{
		config.cpe_uuid = value_ptr;
		return SUCCESS;
	}
}
int cfg_key_time_interval(char *filename, char *value_ptr, char field_arr[MAX_CMD_FIELD_NUM][MAX_CMD_FIELD_LEN], int field_num)
{
	int value = atoi(value_ptr);
	if (value < 0) {
		return ERR;
	}
	config.time_interval= value;
	
	return SUCCESS;
}