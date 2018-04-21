# Makefile.am for Insignia/lib

INCLUDES = \
	-I$(top_builddir)	\
	@PPDFB_CFLAGS@		\
	@PNG_CFLAGS@		\
	@DFBINTERNAL_CFLAGS@

LIBS = \
	@PPDFB_LIBS@	\
	@PNG_LIBS@

AM_CPPFLAGS = \
	-DDATADIR=\"$(DATADIR)/Insignia/\"	\
	-DPACKAGE=\"$(PACKAGE)\"

lib_LTLIBRARIES = libinsignia.la

libinsignia_la_SOURCES = \
	common.h			\
	Image.cpp			\
	Image.h				\
	Insignia.cpp			\
	Insignia.h			\
	InsigniaConfig.cpp		\
	InsigniaConfig.h		\
	InsigniaException.cpp		\
	InsigniaException.h		\
	InsigniaTest.cpp		\
	InsigniaTest.h			\
	InsigniaTestManager.cpp		\
	InsigniaTestManager.h		\
	InsigniaUtil.cpp		\
	InsigniaUtil.h

libinsignia_la_LIBADD = \
	$(LIBS)

libinsignia_la_LDFLAGS = \
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)	\
	-release $(LT_RELEASE)

include $(top_srcdir)/rules/nmfile.make

