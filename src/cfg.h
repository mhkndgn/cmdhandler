#ifndef CFG_H
#define CFG_H

//Global variables
extern char *cfg[LARGEBUFLEN];
extern int rows;

/* structures */
struct _dictionary_line {
  char key[SRVBUFLEN];
  int (*func)(char *, char *, char [MAX_CMD_FIELD_NUM][MAX_CMD_FIELD_LEN], int);
};

//Prototypes
extern int parse_configuration_file(char *filename);
extern void evaluate_configuration(char *filename, int rows);
extern void sanitize_cfg(int rows, char *filename);
extern void trim_spaces(char *buf);
extern int isblankline(char *line);
extern int iscomment(char *line);

#endif