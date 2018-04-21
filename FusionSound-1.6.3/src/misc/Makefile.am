## Makefile.am for FusionSound/src/misc

INCLUDES = \
	-I$(top_builddir)/include	\
	-I$(top_srcdir)/include	\
	-I$(top_srcdir)/src 	\
	$(FUSION_CFLAGS)

AM_CPPFLAGS = \
	-DSYSCONFDIR=\"@SYSCONFDIR@\"

internalincludedir = $(INTERNALINCLUDEDIR)/misc

internalinclude_HEADERS = \
	sound_conf.h \
	sound_util.h

noinst_LTLIBRARIES = \
	libfusionsoundmisc.la

libfusionsoundmisc_la_SOURCES = \
	sound_conf.c	\
	sound_conf.h	\
	sound_util.c	\
	sound_util.h

