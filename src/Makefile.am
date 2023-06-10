bin_PROGRAMS = cmdhandler

cmdhandler_SOURCES = cmdhandler.c cfg.c cfg.h config-handlers.c config-handlers.h utils.c utils.h globals.h defines.h 

cmdhandler_LDADD   = @JANSSON_LIBS@ 
cmdhandler_CFLAGS  = @JANSSON_CFLAGS@

cmdhandler_LDADD  += @KAFKA_LIBS@
cmdhandler_CFLAGS += @KAFKA_CFLAGS@



