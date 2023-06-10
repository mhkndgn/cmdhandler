#ifndef DEFINES_H
#define DEFINES_H

#define SEC2USEC(x) (x*1000*1000)

#define ARGS_COMMANDD "U:f:"
#define CONFIG_FILE_LEN 256
#define SRVBUFLEN 256
#define LARGEBUFLEN 8192
#define VERYSHORTBUFLEN 32
#define MAX_CMD_OUTPUT_LINE_NUM 1024
#define MAX_CMD_OUTPUT_LINE_LEN 1024
#define MAX_CMD_FIELD_NUM 32
#define MAX_CMD_FIELD_LEN 256

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef ERR
#define ERR -1
#endif
#ifndef SUCCESS
#define SUCCESS 0
#endif

#endif