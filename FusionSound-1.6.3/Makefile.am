ACLOCAL_AMFLAGS = -I m4

if BUILD_PROXY
PROXY_DIR=proxy
else
PROXY_DIR=
endif

if BUILD_EXAMPLES
EXAMPLES_DIR=examples
else
EXAMPLES_DIR=
endif

if BUILD_TOOLS
TOOLS_DIR=tools
else
TOOLS_DIR=
endif


SUBDIRS = \
	docs		\
	include		\
	src		\
	drivers	\
	interfaces	\
	$(PROXY_DIR)	\
	$(EXAMPLES_DIR)	\
	$(TOOLS_DIR)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = fusionsound.pc fusionsound-internal.pc

EXTRA_DIST = autogen.sh

