# Makefile.am for Insignia/tests

EXTRA_DIST = \
	DFBTestBlitting.cpp			\
	DFBTestClipping.cpp			\
	DFBTestFX.cpp				\
	DFBTestInit.cpp				\
	DFBTestInterlocks.cpp			\
	DFBTestMulti01.cpp			\
	DFBTestMulti02.cpp			\
	DFBTestMulti03.cpp			\
	DFBTestMulti04.cpp			\
	DFBTestMultiLayerContext.cpp		\
	DFBTestMultiSaWMan.cpp			\
	DFBTestPrimary.cpp			\
	DFBTestRender.h				\
	DFBTestRenderBlit.cpp			\
	DFBTestRenderFill.cpp			\
	DFBTestSimple.h				\
	DFBTestSimpleFont.cpp			\
	DFBTestSimpleWindow.cpp			\
	DFBTestStress01.cpp			\
	DFBTestSurfaceCreate.cpp		\
	DFBTestSurfaceFrameTime.cpp		\
	DFBTestSurfaceFrameTimeMulti.cpp	\
	DFBTestTempSurface.cpp			\
	DFBTestWindowCreate.cpp			\
	DFBfx.cpp				\
	DFBfx.h


DISTCLEANFILES = Io.rules

INCLUDES = \
	-DDATADIR=\"$(DATADIR)/Insignia/\"	\
	-DPACKAGE=\"$(PACKAGE)\"	\
	-I$(top_builddir)		\
	-I$(top_srcdir)/lib		\
	@PPDFB_CFLAGS@			\
	@DFBINTERNAL_CFLAGS@

LIBS = 

ALL_TESTS = \
	DFBTestBlitting			\
	DFBTestClipping			\
	DFBTestFX			\
	DFBTestInit			\
	DFBTestInterlocks		\
	DFBTestMisc			\
	DFBTestMulti01			\
	DFBTestMulti02			\
	DFBTestMulti03			\
	DFBTestMulti04			\
	DFBTestMultiLayerContext	\
	DFBTestPrimary			\
	DFBTestRenderBlit		\
	DFBTestRenderFill		\
	DFBTestSimpleFont		\
	DFBTestSimpleWindow		\
	DFBTestSurfaceFrameTime		\
	DFBTestSurfaceFrameTimeMulti	\
	DFBTestStress01			\
	DFBTestTempSurface

DFBTestBlitting_SRCS = \
	DFBfx.cpp			\
	DFBfx.h				\
	DFBTestBlitting.cpp

DFBTestClipping_SRCS = \
	DFBTestClipping.cpp

DFBTestFX_SRCS = \
	DFBfx.cpp			\
	DFBfx.h				\
	DFBTestFX.cpp

DFBTestInit_SRCS = \
	DFBTestInit.cpp

DFBTestInterlocks_SRCS = \
	DFBTestInterlocks.cpp

DFBTestMisc_SRCS = \
	DFBTestSurfaceCreate.cpp	\
	DFBTestWindowCreate.cpp

DFBTestMulti01_SRCS = \
	DFBTestMulti01.cpp

DFBTestMulti02_SRCS = \
	DFBTestMulti02.cpp

DFBTestMulti03_SRCS = \
	DFBTestMulti03.cpp

DFBTestMulti04_SRCS = \
	DFBTestMulti04.cpp

DFBTestMultiLayerContext_SRCS = \
	DFBTestMultiLayerContext.cpp

DFBTestPrimary_SRCS = \
	DFBTestPrimary.cpp

DFBTestRenderBlit_SRCS = \
	DFBTestRenderBlit.cpp	\
	DFBTestRender.h

DFBTestRenderFill_SRCS = \
	DFBTestRenderFill.cpp	\
	DFBTestRender.h

DFBTestSurfaceFrameTime_SRCS = \
	DFBTestSurfaceFrameTime.cpp

DFBTestSurfaceFrameTimeMulti_SRCS = \
	DFBTestSurfaceFrameTimeMulti.cpp

DFBTestStress01_SRCS = \
	DFBTestStress01.cpp

DFBTestSimpleFont_SRCS = \
	DFBTestSimpleFont.cpp

DFBTestSimpleWindow_SRCS = \
	DFBTestSimpleWindow.cpp	\
	DFBTestSimple.h

DFBTestTempSurface_SRCS = \
	DFBTestTempSurface.cpp


ALL_MODULES = $(ALL_TESTS:%=%.Io)

all-local:: $(ALL_MODULES)

clean-local::
	rm -f $(ALL_MODULES) Io.rules

install-exec-local:
	install -d $(DESTDIR)/$(libdir)/Insignia
	for i in $(ALL_MODULES); do \
		install -c -m 644 $$i $(DESTDIR)/$(libdir)/Insignia/$$i; \
	done

Io.rules: Makefile.am
	(\
	for Test in $(ALL_TESTS); do \
		echo "$$Test.Io: \$$($${Test}_SRCS) \$$(top_srcdir)/lib/InsigniaConfig.h \$$(top_srcdir)/lib/InsigniaException.h \$$(top_srcdir)/lib/InsigniaTest.h \$$(top_srcdir)/lib/InsigniaUtil.h"; \
		echo "	\$$(CXX) -fPIC -shared -o \$$@ \$$(CPPFLAGS) \$$(CXXFLAGS) \$$(INCLUDES) \$$($${Test}_SRCS) \$$(LIBS)"; \
		echo; \
	done; \
	echo; \
	echo 'ALL_SOURCES = \'; \
	for Test in $(ALL_TESTS); do \
		echo "	\$$($${Test}_SRCS) \\"; \
	done; \
	echo; \
	) > $@

-include Io.rules
