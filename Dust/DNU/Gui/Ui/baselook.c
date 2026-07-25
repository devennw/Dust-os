#include <gtk/gtk.h>

static void load_css(void){

    GtkCssProvider *provider = gtk_css_provider_new();

  // style css modern operating system
  gtk_css_provider_load_from_path(provider, "style.css");
    
  // styling fo all of the monitor
  gtk_style_context_add_provider_for_display(
      gdk_style_get_default(),
      GTK_STYLE_PROVIDER(provider),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
      );
  g_object_unref(provider);
}
