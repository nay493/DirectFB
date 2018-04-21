## Makefile.am for FusionSound/proxy/dispatcher

INTERFACES_DIR = $(MODULEDIR)/interfaces

ifusionsounddir         = $(INTERFACES_DIR)/IFusionSound
ifusionsoundstreamdir   = $(INTERFACES_DIR)/IFusionSoundStream
ifusionsoundbufferdir   = $(INTERFACES_DIR)/IFusionSoundBuffer
ifusionsoundplaybackdir = $(INTERFACES_DIR)/IFusionSoundPlayback

INCLUDES = \
	-I$(top_srcdir)/include		\
	-I$(top_srcdir)/src 		\
	-I$(top_srcdir)/proxy/compressor	\
	-I$(top_srcdir)/proxy/requestor	\
	-DDATADIR=\"${RUNTIME_SYSROOT}@DATADIR@\" \
	$(VOODOO_CFLAGS)

LIBS = ../../src/libfusionsound.la $(VOODOO_LIBS)


ifusionsound_LTLIBRARIES = libifusionsound_dispatcher.la

ifusionsoundstream_LTLIBRARIES = libifusionsoundstream_dispatcher.la

ifusionsoundbuffer_LTLIBRARIES = libifusionsoundbuffer_dispatcher.la

ifusionsoundplayback_LTLIBRARIES = libifusionsoundplayback_dispatcher.la


libifusionsound_dispatcher_la_SOURCES = ifusionsound_dispatcher.c \
								ifusionsound_dispatcher.h
libifusionsound_dispatcher_la_LDFLAGS = -avoid-version -module
libifusionsound_dispatcher_la_LIBADD = $(LIBS)

libifusionsoundstream_dispatcher_la_SOURCES = ifusionsoundstream_dispatcher.c \
									 ifusionsoundstream_dispatcher.h
libifusionsoundstream_dispatcher_la_LDFLAGS = -avoid-version -module
libifusionsoundstream_dispatcher_la_LIBADD = $(LIBS) ../compressor/libfusionsound_compressor.la

libifusionsoundbuffer_dispatcher_la_SOURCES = ifusionsoundbuffer_dispatcher.c \
									 ifusionsoundbuffer_dispatcher.h
libifusionsoundbuffer_dispatcher_la_LDFLAGS = -avoid-version -module
libifusionsoundbuffer_dispatcher_la_LIBADD = $(LIBS) ../compressor/libfusionsound_compressor.la

libifusionsoundplayback_dispatcher_la_SOURCES = ifusionsoundplayback_dispatcher.c \
									   ifusionsoundplayback_dispatcher.h
libifusionsoundplayback_dispatcher_la_LDFLAGS = -avoid-version -module
libifusionsoundplayback_dispatcher_la_LIBADD = $(LIBS)
