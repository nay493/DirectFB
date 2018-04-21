## Makefile.am for FusionSound/tools

INCLUDES = \
	-I$(top_builddir)/include	\
	-I$(top_srcdir)/include		\
	-I$(top_srcdir)/src		\
	$(FUSION_CFLAGS)	\
	$(VOODOO_CFLAGS)

if BUILD_PROXY
PROXY_PROGS = fsproxy
else
PROXY_PROGS =
endif

bin_PROGRAMS = fsdump fsmaster fsplay fsvolume $(PROXY_PROGS)

fsdump_SOURCES = fsdump.c
fsdump_LDADD   = ../src/libfusionsound.la $(FUSION_LIBS)

fsmaster_SOURCES = fsmaster.c
fsmaster_LDADD   = ../src/libfusionsound.la $(FUSION_LIBS)

fsplay_SOURCES = fsplay.c
fsplay_LDADD   = ../src/libfusionsound.la $(FUSION_LIBS)

fsvolume_SOURCES = fsvolume.c
fsvolume_LDADD   = ../src/libfusionsound.la $(FUSION_LIBS)

fsproxy_SOURCES = fsproxy.c
fsproxy_LDADD = ../src/libfusionsound.la $(FUSION_LIBS) $(VOODOO_LIBS)


EXTRA_DIST = gendoc.pl
