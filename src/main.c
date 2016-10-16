#include <stdlib.h>
#include <gtk/gtk.h>

static void clipboard_change_callback(GtkClipboard *buf, GdkEventOwnerChange *event, gpointer data);

GtkClipboard* primary;
GtkClipboard* clipboard;

gchar* primary_save = NULL;
gchar* clipboard_save = NULL;

int main(int argc, char** argv) {
  gtk_init(&argc, &argv);

  primary = gtk_clipboard_get(GDK_SELECTION_PRIMARY);
  clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

  g_signal_connect(primary, "owner_change", G_CALLBACK(clipboard_change_callback), NULL);
  g_signal_connect(clipboard, "owner_change", G_CALLBACK(clipboard_change_callback), NULL);

  gtk_main();

  return EXIT_FAILURE;
}

static void clipboard_change_callback(GtkClipboard *buf, GdkEventOwnerChange *event, gpointer data) {
  (void)data;
  g_assert(buf);
  g_assert(event);

  if (buf == primary) {
    gchar* text = gtk_clipboard_wait_for_text(buf);

    if (g_strcmp0(primary_save, text)) {
      gtk_clipboard_set_text(clipboard, text, -1);
      g_free(primary_save);
      primary_save = text;
    }
  } else if (buf == clipboard) {
    gchar* text = gtk_clipboard_wait_for_text(buf);

    if (g_strcmp0(clipboard_save, text)) {
      gtk_clipboard_set_text(primary, text, -1);
      g_free(clipboard_save);
      clipboard_save = text;
    }
  }
}
