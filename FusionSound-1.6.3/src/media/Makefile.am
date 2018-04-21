## Makefile.am for FusionSound/src/media

INCLUDES = \
	-I$(top_srcdir)/include	\
	-I$(top_srcdir)/src 	\
	$(FUSION_CFLAGS)

internalincludedir = $(INTERNALINCLUDEDIR)/media

internalinclude_HEADERS = \
	ifusionsoundmusicprovider.h

noinst_LTLIBRARIES = \
	libfusionsoundmedia.la

libfusionsoundmedia_la_SOURCES = \
	ifusionsoundmusicprovider.h \
	ifusionsoundmusicprovider.c

