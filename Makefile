CC     = gcc
PKGS   = gtk+-3.0 gtk-layer-shell-0 appindicator3-0.1
CFLAGS = $(shell pkg-config --cflags $(PKGS)) -O2 -Wall -Wextra
LIBS   = $(shell pkg-config --libs $(PKGS))

crosshair: src/crosshair.c
	$(CC) -o crosshair src/crosshair.c $(CFLAGS) $(LIBS)

clean:
	rm -f crosshair
