// GTK4/libadwaita application entry point
#include <adwaita.h>
#include "AppState.hpp"
#include "MainWindow.hpp"

using namespace Huenicorn::Gtk;

static void activate(GtkApplication* app, gpointer user_data) {
    (void)user_data;
    
    // Use system color scheme (supports dark mode properly)
    AdwStyleManager* style_manager = adw_style_manager_get_default();
    adw_style_manager_set_color_scheme(style_manager, ADW_COLOR_SCHEME_PREFER_DARK);
    
    setState(new AppState());
    setupMainWindow(app);
}

int main(int argc, char* argv[])
{
    AdwApplication* app = adw_application_new("io.github.music.huenicorn", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), nullptr);
    
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    
    cleanupState();
    g_object_unref(app);
    
    return status;
}

