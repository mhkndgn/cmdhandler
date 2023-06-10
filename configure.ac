AC_INIT([cmdhandler], [1.0], [])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])

PKG_CHECK_MODULES([KAFKA], [rdkafka >= 0.9.2], [
	_save_LIBS="$LIBS"
	LIBS="$LIBS $KAFKA_LIBS"
	AC_CHECK_LIB([rdkafka], [rd_kafka_produce])
	LIBS="$_save_LIBS"
	_save_CFLAGS="$CFLAGS"
	CFLAGS="$CFLAGS $KAFKA_CFLAGS"
	AC_CHECK_HEADER([librdkafka/rdkafka.h])
	CFLAGS="$_save_CFLAGS"
	echo "KAFKA_LIBS=$KAFKA_LIBS KAFKA_CFLAGS=$KAFKA_CFLAGS"
	], [
	AC_MSG_ERROR([Missing librdkafka. Refer to: https://github.com/edenhill/librdkafka/])
])

PKG_CHECK_MODULES([JANSSON], [jansson >= 2.5], [
	_save_LIBS="$LIBS"
	LIBS="$LIBS $JANSSON_LIBS"
	AC_CHECK_LIB([jansson], [json_object])
	LIBS="$_save_LIBS"
	_save_CFLAGS="$CFLAGS"
	CFLAGS="$CFLAGS $JANSSON_CFLAGS"
	AC_CHECK_HEADER([jansson.h])
	CFLAGS="$_save_CFLAGS"
	echo "JANSSON_LIBS=$JANSSON_LIBS JANSSON_CFLAGS=$JANSSON_CFLAGS"
	], [
	AC_MSG_ERROR([Missing libjansson. Refer to: http://www.digip.org/jansson/])
])

AC_OUTPUT
