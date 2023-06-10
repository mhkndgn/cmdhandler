#include <time.h>
#include <string.h>
#include <ctype.h>
#include "defines.h"
#include "utils.h"

char *compose_json_str(json_t *obj)
{
  char *tmpbuf = NULL;

  tmpbuf = json_dumps(obj, JSON_PRESERVE_ORDER);
  json_decref(obj);
  return tmpbuf;
}

void compose_timestamp(char *buf, int buflen, struct timeval *tv)
{
	int slen;
	time_t time1;
	struct tm *time2;

	if (buflen < VERYSHORTBUFLEN) return; 

	time1 = tv->tv_sec;
	time2 = localtime(&time1);

	slen = strftime(buf, buflen, "%Y-%m-%d %H:%M:%S", time2);

	//printf((buf + slen), (buflen - slen), ".%.6ld", (long)tv->tv_usec); //for usec
	
}

void compose_json_timestamp(json_t *obj)
{
	char tstamp_str[VERYSHORTBUFLEN];
	struct timeval tv;
	tv.tv_sec = time(NULL);
	tv.tv_usec = 0;

	compose_timestamp(tstamp_str, VERYSHORTBUFLEN, &tv);
	json_object_set_new_nocheck(obj, "timestamp", json_string(tstamp_str));
}

char *compose_json_command(json_t *obj, char *cmd_name, char *cmd_output)
{
	json_object_set_new(obj, cmd_name, json_string(cmd_output));
}

char *compose_json_uuid(json_t *obj, char *cpe_uuid)
{
	json_object_set_new(obj, "uuid",json_string(cpe_uuid));
}

void parse_output(json_t *obj, char *field, char *cmd_output)
{
	char *next_line;
	char *empty = "";
	next_line = strchr(cmd_output,'\n');
	if(next_line){
		*next_line = *empty;
	}
	json_object_set_new(obj, field, json_string(cmd_output));
}

void parse_output_with_fields(json_t *obj, char *cmd_output, char fields[MAX_CMD_FIELD_NUM][MAX_CMD_FIELD_LEN], int field_num)
{
	char *delim, *temp, *next_line;
	char *empty = "";
	int last_output = FALSE;
	
	temp = cmd_output;

	next_line = strchr(temp,'\n');
	if(next_line){
		*next_line = *empty;
	}
	
	for(int i=0; i<field_num; i++){
		delim = strchr(temp,' ');
		if(delim){
			*delim = '\0';
			compose_json_command(obj, fields[i], temp);
			temp = delim +1;
		}
		else{
			if(!last_output){
				compose_json_command(obj, fields[i], temp);
				last_output = TRUE;
			}
			else{
				compose_json_command(obj, fields[i], empty);
			}
		}
	}
}

int iscomment(char *line)
{
  int len, j, first_char = TRUE;

  if (!line) return FALSE;

  len = strlen(line);
  for (j = 0; j <= len; j++) {
    if (!isspace(line[j])) first_char--;
    if (!first_char) {
      if (line[j] == '!' || line[j] == '#') return TRUE; 
      else return FALSE;
    }
  }

  return FALSE;
}

int isblankline(char *line)
{
  int len, j, n_spaces = 0;
 
  if (!line) return FALSE;

  len = strlen(line); 
  for (j = 0; j < len; j++) 
    if (isspace(line[j])) n_spaces++;

  if (n_spaces == len) return TRUE;
  else return FALSE;
}

void trim_spaces(char *buf)
{
  char *tmp_buf;
  int i, len;

  len = strlen(buf);

  tmp_buf = (char *)malloc(len + 1);
  if (tmp_buf == NULL) {
    fprintf(stderr, "ERROR: trim_spaces: malloc() failed.\n");
    return;
  }
   
  /* trimming spaces at beginning of the string */
  for (i = 0; i <= len; i++) {
    if (!isspace(buf[i])) {
      if (i != 0) { 
        strncpy(tmp_buf, &buf[i], len+1-i);
        strncpy(buf, tmp_buf, len+1-i);
      }
      break;
    } 
  }

  /* trimming spaces at the end of the string */
  for (i = strlen(buf)-1; i >= 0; i--) { 
    if (isspace(buf[i]))
      buf[i] = '\0';
    else break;
  }

  free(tmp_buf);
}