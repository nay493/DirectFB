# Makefile.am for Insignia/src

INCLUDES = \
	-I$(top_srcdir)/lib			\
	@PPDFB_CFLAGS@				\
	@DFBINTERNAL_CFLAGS@

AM_CPPFLAGS = \
	-DDATADIR=\"$(DATADIR)/Insignia/\"	\
	-DPACKAGE=\"$(PACKAGE)\"		\
	-DMODULEDIR=\"$(libdir)/Insignia\"

bin_PROGRAMS = \
	Insignia

bin_SCRIPTS = \
	InsigniaRun

EXTRA_DIST = \
	InsigniaRun

Insignia_SOURCES = \
	main.cpp

Insignia_LDADD = \
	$(top_srcdir)/lib/libinsignia.la	\
	@PPDFB_LIBS@ -ldl
