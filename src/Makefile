NULL=
font_manager_SOURCES=\
	fm-main.c \
	fm-preview-list.c \
	fm-preview-model.c \
	fm-window.c \
	gtk-helpers.c \
	herzi-main-window.c \
	$(NULL)
font_manager_OBJECTS=$(font_manager_SOURCES:.c=.o)

all: font-manager

font-manager: $(font_manager_OBJECTS)
	gcc -o $@ $^ -g $(shell pkg-config --libs gtk+-2.0 fontconfig libglade-2.0 freetype2)

%.o: %.c
	gcc -c -o $@ $< -g $(shell pkg-config --cflags gtk+-2.0 fontconfig libglade-2.0 freetype2)

clean:
	rm -rf font-manager *.o

