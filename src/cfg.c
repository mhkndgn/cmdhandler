#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "defines.h"
#include "cfg.h"
#include "config-handlers.h"
#include "globals.h"

//Global variables
char *cfg[LARGEBUFLEN];
int rows;

static const struct _dictionary_line dictionary[] = {
	{"uuid", cfg_key_cpe_uuid}, 
	{"kafka_broker_host", cfg_key_kafka_host}, 
	{"kafka_topic", cfg_key_kafka_topic},
	{"interval_sec", cfg_key_time_interval},
	{"command", add_command},
	{"", NULL}
};

void sanitize_cfg(int rows, char *filename)
{
  int rindex = 0, len;
  char localbuf[10240];

  while (rindex < rows) {
    memset(localbuf, 0, 10240);

    /* checking the whole line: if it's a comment starting with
       '!', it will be removed */
    if (iscomment(cfg[rindex])) memset(cfg[rindex], 0, strlen(cfg[rindex]));

    /* checking the whole line: if it's void, it will be removed */
    if (isblankline(cfg[rindex])) memset(cfg[rindex], 0, strlen(cfg[rindex]));

    /* 
       a pair of syntax checks on the whole line:
       - does the line contain at least a ':' verb ?
       - are the square brackets weighted both in key and value ?
    */
    len = strlen(cfg[rindex]);
    if (len) {
      if (!strchr(cfg[rindex], ':')) {
	fprintf(stderr, "ERROR: [%s:%u] Syntax error: missing ':'. Exiting.\n", filename, rindex+1); 
	exit(1);
      }
    }

    /* checking the whole line: erasing unwanted spaces from key;
       trimming start/end spaces from value; symbols will be left
       untouched */
    len = strlen(cfg[rindex]);
    if (len) {
      int symbol = FALSE, value = FALSE, cindex = 0, lbindex = 0;
      char *valueptr = NULL;

      while(cindex <= len) {
	if (!value) {
          if (cfg[rindex][cindex] == '[') symbol++;
          else if (cfg[rindex][cindex] == ']') symbol--;
	  else if (cfg[rindex][cindex] == ':') {
	    value++;
	    valueptr = &localbuf[lbindex+1];
	  }
	}
        if ((!symbol) && (!value)) {
	  if (!isspace(cfg[rindex][cindex])) {
	    localbuf[lbindex] = cfg[rindex][cindex]; 
	    lbindex++;
	  }
        }
        else {
	  localbuf[lbindex] = cfg[rindex][cindex];
	  lbindex++;
        }
        cindex++;
      }
      localbuf[lbindex] = '\0';
      trim_spaces(valueptr);
      strcpy(cfg[rindex], localbuf);
    }

    /* checking key field: does a key still exist ? */
    len = strlen(cfg[rindex]);
    if (len) {
      if (cfg[rindex][0] == ':') {
	fprintf(stderr, "ERROR: [%s:%u] Syntax error: missing key. Exiting.\n", filename, rindex+1);
	exit(1);
      }
    }

    /* checking key field: converting key to lower chars */ 
    len = strlen(cfg[rindex]);
    if (len) {
      int symbol = FALSE, cindex = 0;

      while(cindex <= len) {
        if (cfg[rindex][cindex] == '[') symbol++;
	else if (cfg[rindex][cindex] == ']') symbol--;

	if (cfg[rindex][cindex] == ':') break;
	if (!symbol) {
	  if (isalpha(cfg[rindex][cindex]))
	    cfg[rindex][cindex] = tolower(cfg[rindex][cindex]);
	}
	cindex++;
      }
    }

    rindex++;
  }
}

void evaluate_configuration(char *filename, int rows)
{
	char *key, *value, *name, *delim, *temp, *temp2;
	int index = 0, dindex, valid_line, key_found = 0, res;
	char fields[MAX_CMD_FIELD_NUM][MAX_CMD_FIELD_LEN];

	while (index < rows) {
		if (*cfg[index] == '\0') valid_line = FALSE;
		else valid_line = TRUE; 

		if (valid_line) {
			/* debugging the line if required */
			//fprintf(stdout, "DEBUG: [%s] %s\n", filename, cfg[index]);

			/* splitting key, value and name */
			delim = strchr(cfg[index], ':');
			*delim = '\0';
			key = cfg[index];
			value = delim+1;

			delim = strchr(key,'[');
			int field_num = 0;
			while(delim){

				*delim = '\0';
				temp = delim + 1;
				
				delim = strchr(temp, ']');
				
				*delim = '\0';
				temp2 = delim+1;
				
				strncpy(fields[field_num],temp,MAX_CMD_FIELD_LEN);
				
				temp = temp2;
				field_num++;
				delim = strchr(temp,'[');
			}

			for (dindex = 0; strcmp(dictionary[dindex].key, ""); dindex++) {
				if (!strcmp(dictionary[dindex].key, key)) {
					res = FALSE;
					if ((*dictionary[dindex].func)) {
						res = (*dictionary[dindex].func)(filename, value, fields, field_num);
						if (res < 0) {
							fprintf(stdout,  "WARN: [%s:%u] Invalid value. Ignored.\n", filename, index+1);
						}
					}
					else fprintf(stdout, "WARN: [%s:%u] Unable to handle key: %s. Ignored.\n", filename, index+1, key);
					key_found = TRUE;
					break;
				}
				else key_found = FALSE;
			}
			
			if (!key_found) fprintf(stdout, "WARN: [%s:%u] Unknown key: %s. Ignored.\n", filename, index+1, key);
		}

		index++;
	}
}

void initialize_configuration(void){
	cmd_num = 0;
	memset(&commands, 0, sizeof(commands));
	memset(&config, 0, sizeof(struct configuration));
}

int parse_configuration_file(char *filename)
{
  char localbuf[10240];
  FILE *file;
  int num = 0, idx;
  rows = 0;
  
  /* NULL filename means we don't have a configuration file; 1st stage: read from
     file and store lines into a first char* array; merge commandline options, if
     required, placing them at the tail - in order to override directives placed
     in the configuration file */
  if (filename) { 
    if ((file = fopen(filename,"r")) == NULL) {
      fprintf(stderr, "ERROR: [%s] file not found.\n", filename);
      return ERR;
    }
    else {
      while (!feof(file)) {
        if (rows == LARGEBUFLEN) {
	  fprintf(stderr, "ERROR: [%s] maximum number of %d lines reached.\n", filename, LARGEBUFLEN);
	  break;
        }
	memset(localbuf, 0, sizeof(localbuf));
        if (fgets(localbuf, sizeof(localbuf), file) == NULL) break;	
        else {
	  localbuf[sizeof(localbuf)-1] = '\0';
          cfg[rows] = malloc(strlen(localbuf)+2);
	  if (!cfg[rows]) {
	    fprintf(stderr, "ERROR: [%s] malloc() failed (parse_configuration_file). Exiting.\n", filename);
	    exit(1);
	  }
          strcpy(cfg[rows], localbuf);
          cfg[rows][strlen(localbuf)+1] = '\0';
          rows++;
        } 
      }
    }
    fclose(file);
  }


  /* 2nd stage: sanitize lines */
  sanitize_cfg(rows, filename);

  
  /* 5th stage: parsing keys and building configurations */ 
  evaluate_configuration(filename, rows);

  return SUCCESS;
}