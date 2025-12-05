// Screen Partition Widget for GTK4
#pragma once

#include <adwaita.h>

namespace Huenicorn::Gtk {

// Draw function for the screen partition area
void drawScreenPartition(GtkDrawingArea* area, cairo_t* cr, 
                         int width, int height, gpointer user_data);

} // namespace Huenicorn::Gtk
