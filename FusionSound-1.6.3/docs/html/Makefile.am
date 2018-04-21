EXTRA_DIST = directfb.png

htmldir = \
	$(datadir)/doc/fusionsound/html/

docs_headers = \
	$(top_srcdir)/include/fusionsound.h

all-local: stamp-docs

stamp-docs: $(top_srcdir)/tools/gendoc.pl $(docs_headers) Makefile.am
	rootme=`pwd`; \
	$(PERL) $(top_srcdir)/tools/gendoc.pl $(FUSIONSOUND_VERSION) $(docs_headers) \
	&& echo timestamp > $$rootme/stamp-docs

install-data-local: stamp-docs
	mkdir -p $(DESTDIR)$(htmldir)
	$(INSTALL_DATA) *.html $(DESTDIR)$(htmldir)
	$(INSTALL_DATA) $(srcdir)/*.png $(DESTDIR)$(htmldir)

distclean-local:
	rm -f *.html stamp-docs
