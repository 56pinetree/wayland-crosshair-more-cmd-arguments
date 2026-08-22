#include <gtk/gtk.h>
#include <gtk-layer-shell.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "tray/tray_linux.c"

#define ICON_FILENAME "crosshair-icon.png"

GtkApplication *app;

struct tray tray;
static void quit_cb(struct tray_menu *item) {
    (void)item;
    tray_exit();
    g_application_quit(G_APPLICATION(app));
}

char icon_path[PATH_MAX];

double height = 0.5;
double width = 0.5;
double radius = 2.0;
// Blue for initial color
double r = 0.0;
double g = 0.6;
double b = 0.9;

void set_icon_path() {
    char exe_path[PATH_MAX];
    ssize_t len;
    len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        perror("readlink failed");
        //return EXIT_FAILURE;
    }

    // Remove the name of the binary leaving just the directory
    int offset = 0;
    int total_offset = 0;
    for(int i = 0; i < len; i++) {
	total_offset++;
	if(exe_path[i] == '/')
	    offset = total_offset;
    }
    len = offset;

    // Append ICON_FILENAME to the path
    exe_path[len-1] = '/';
    int lo = sizeof(ICON_FILENAME);
    len += lo;
    for(int i = len-lo; i <= len; i++) {
	exe_path[i] = ICON_FILENAME[lo - (len - i)];
    }
    exe_path[len] = '\0';

    for(int i = 0; i < PATH_MAX; i++)
	icon_path[i] = exe_path[i];
}

static gboolean on_draw(GtkWidget *w, cairo_t *cr, gpointer _) {
    (void)_;
    GtkAllocation a;
    gtk_widget_get_allocation(w, &a);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba(cr, 0, 0, 0, 0);
    cairo_paint(cr);
    cairo_set_source_rgb(cr, r, g, b);
    cairo_arc(cr, a.width * width, a.height * (1.0 - height), radius, 0, 2 * G_PI);
    cairo_fill(cr);


    return FALSE;
}

static void activate(GtkApplication *app, gpointer _) {
    (void)_;
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_application_add_window(GTK_APPLICATION(app), GTK_WINDOW(win));
    gtk_window_set_decorated(GTK_WINDOW(win), FALSE);

    GdkScreen *screen = gtk_window_get_screen(GTK_WINDOW(win));
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    if (visual) gtk_widget_set_visual(win, visual);
    gtk_widget_set_app_paintable(win, TRUE);

    gtk_layer_init_for_window(GTK_WINDOW(win));
    gtk_layer_set_layer(GTK_WINDOW(win), GTK_LAYER_SHELL_LAYER_OVERLAY);
    gtk_layer_set_keyboard_mode(GTK_WINDOW(win), GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);
    gtk_layer_set_anchor(GTK_WINDOW(win), GTK_LAYER_SHELL_EDGE_LEFT,   TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(win), GTK_LAYER_SHELL_EDGE_RIGHT,  TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(win), GTK_LAYER_SHELL_EDGE_TOP,    TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(win), GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
    /* -1 = ignore exclusive zones from bars/docksla, use full screen dimensions */
    gtk_layer_set_exclusive_zone(GTK_WINDOW(win), -1);

    GtkWidget *da = gtk_drawing_area_new();
    g_signal_connect(da, "draw", G_CALLBACK(on_draw), NULL);
    gtk_container_add(GTK_CONTAINER(win), da);

    gtk_widget_show_all(win);

    cairo_region_t *empty = cairo_region_create();
    gdk_window_input_shape_combine_region(gtk_widget_get_window(win), empty, 0, 0);

    if (tray_init(&tray) < 0) {
	printf("failed to create tray\n");
	return;
    }
}

void set_value(int index, double value) {
    switch(index) {
	case 1:
	    height = value;
	    break;
	case 2:
	    width = value;
	    break;
	case 3:
	    radius = value;
	    break;
	case 4:
	    r = value;
	    break;
	case 5:
	    g = value;
	    break;
	case 6:
	    b = value;
	    break;
    }
}


int main(int argc, char *argv[]) {
    if(argc == 2 && argv[1][0] == '-') {
        printf("usage: crosshair [height] [width] [radius] [r] [g] [b]\n");
        printf("example: crosshair 0.5, 0.5, 2.0, 0.0, 0.6, 0.9\n");
        printf("\nNote: all arguments must be between 0.0 and 1.0\n");
        return 0;
    }
    if(argc > 1) {
        for(int i = 0; i < argc; i++) {
            if(argv[i][0] == 'x') continue;
            set_value(i, atof(argv[i]));
        }
    }

    set_icon_path();
    tray = (struct tray) {
	.icon = icon_path,
	.menu =
	    (struct tray_menu[]) {
		{.text = "Quit", .cb = quit_cb},
		{.text = NULL}},
    };

    app = gtk_application_new("se.n1k0.crosshair", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);
    return status;
}
