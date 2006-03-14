NULL=
gladedir=$(pkgdatadir)
glade_DATA=font-manager.glade

applicationdir = $(datadir)/applications
application_in_files = font-manager.desktop.in
application_DATA = $(application_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST=\
	$(application_in_files) \
	$(glade_DATA) \
	$(NULL)
CLEANFILES=\
	$(application_DATA) \
	$(NULL)
