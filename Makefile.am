
ACLOCAL_AMFLAGS = -I build-aux/m4

bin_PROGRAMS =
CLEANFILES =
include_HEADERS =
lib_LTLIBRARIES =

if DEBUG
AM_CFLAGS = -DDEBUG -O0 -ggdb -ansi -pedantic
else
AM_CFLAGS = -DNDEBUG -O2 -ansi -pedantic
endif

bin_PROGRAMS += ansicl
ansicl_LDADD = -lreadline -lncurses -lgc
ansicl_SOURCES = \
	ansicl.c \
	backtrace.c \
	block.c \
	env.c \
	error.c \
	eval.c \
	form.c \
	form_string.c \
	frame.c \
	lambda.c \
	package.c \
	print.c \
	read.c \
	skiplist.c \
	tags.c \
	unwind_protect.c
