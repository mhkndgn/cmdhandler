#ifndef GLOBALS_H
#define GLOBALS_H

#include "defines.h"


struct command {
  char *cmd;
  char cmd_fields[MAX_CMD_FIELD_NUM][MAX_CMD_FIELD_LEN];
  int cmd_fields_num;
};

struct command commands[SRVBUFLEN];

struct output {
  char **out_lines;
  int line_num;
};

int cmd_num;

struct configuration {
	char *cpe_uuid;
	char *kafka_topic;
	char *kafka_host; 
	int time_interval;
};

struct configuration config; 



#endif
