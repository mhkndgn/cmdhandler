#ifndef UTILS_H
#define UTILS_H

#include <jansson.h>

extern char *compose_json_str(json_t *obj);
extern char *compose_json_command(json_t *obj, char *cmd_name, char *cmd_output);
extern char *compose_json_uuid(json_t *obj, char *cpe_uuid);
extern void parse_output(json_t *obj, char *field, char *cmd_output);
extern void parse_output_with_fields(json_t *obj, char *cmd_output, char fields[MAX_CMD_FIELD_NUM][MAX_CMD_FIELD_LEN], int field_num);
extern void compose_json_timestamp(json_t *obj);
extern void compose_timestamp(char *buf, int buflen, struct timeval *tv);
extern int iscomment(char *line);
extern int isblankline(char *line);
extern void trim_spaces(char *buf);

#endif