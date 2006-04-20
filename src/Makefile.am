NULL=
lib_LTLIBRARIES=libfont-manager.la
libfont_manager_la_SOURCES=\
	fm-preview-list.c \
	fm-preview-list.h \
	fm-preview-model.c \
	fm-preview-model.h \
	fm-settings.c \
	fm-settings.h \
	fm-window.c \
	fm-window.h \
	glib-helpers.h \
	gtk-helpers.c \
	gtk-helpers.h \
	herzi-main-window.c \
	herzi-main-window.h \
	$(NULL)
libfont_manager_la_LIBADD=\
	$(CORE_LIBS) \
	$(NULL)

bin_PROGRAMS=font-manager
font_manager_SOURCES=\
	fm-main.c \
	$(NULL)
font_manager_LDADD=\
	$(libfont_manager_la_LIBADD) \
	libfont-manager.la \
	$(NULL)

noinst_PROGRAMS=camarilla
camarilla_LDADD=\
	$(CAMARILLA_LIBS) \
	$(libfont_manager_la_LIBADD) \
	libfont-manager.la \
	$(NULL)

AM_CPPFLAGS=\
	$(CAMARILLA_CFLAGS) \
	$(NULL)

