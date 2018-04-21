## Makefile.am for FusionSound/proxy/requestor

INTERFACES_DIR = $(MODULEDIR)/interfaces

ifusionsounddir         = $(INTERFACES_DIR)/IFusionSound
ifusionsoundstreamdir   = $(INTERFACES_DIR)/IFusionSoundStream
ifusionsoundbufferdir   = $(INTERFACES_DIR)/IFusionSoundBuffer
ifusionsoundplaybackdir = $(INTERFACES_DIR)/IFusionSoundPlayback

INCLUDES = \
	-I$(top_srcdir)/include			\
	-I$(top_srcdir)/src 			\
	-I$(top_srcdir)/proxy/compressor	\
	-I$(top_srcdir)/proxy/dispatcher	\
	-DDATADIR=\"${RUNTIME_SYSROOT}@DATADIR@\" \
	$(VOODOO_CFLAGS)

LIBS = ../../src/libfusionsound.la $(VOODOO_LIBS) 


ifusionsound_LTLIBRARIES = libifusionsound_requestor.la

ifusionsoundstream_LTLIBRARIES = libifusionsoundstream_requestor.la

ifusionsoundbuffer_LTLIBRARIES = libifusionsoundbuffer_requestor.la

ifusionsoundplayback_LTLIBRARIES = libifusionsoundplayback_requestor.la


libifusionsound_requestor_la_SOURCES = ifusionsound_requestor.c
libifusionsound_requestor_la_LDFLAGS = -avoid-version -module
libifusionsound_requestor_la_LIBADD  = $(LIBS)

libifusionsoundstream_requestor_la_SOURCES = ifusionsoundstream_requestor.c
libifusionsoundstream_requestor_la_LDFLAGS = -avoid-version -module
libifusionsoundstream_requestor_la_LIBADD  = $(LIBS) ../compressor/libfusionsound_compressor.la

libifusionsoundbuffer_requestor_la_SOURCES = ifusionsoundbuffer_requestor.c
libifusionsoundbuffer_requestor_la_LDFLAGS = -avoid-version -module
libifusionsoundbuffer_requestor_la_LIBADD  = $(LIBS) ../compressor/libfusionsound_compressor.la

libifusionsoundplayback_requestor_la_SOURCES = ifusionsoundplayback_requestor.c
libifusionsoundplayback_requestor_la_LDFLAGS = -avoid-version -module
libifusionsoundplayback_requestor_la_LIBADD  = $(LIBS)
