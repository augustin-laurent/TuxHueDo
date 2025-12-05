// Screen Partition Widget Implementation
#include "ScreenPartitionWidget.hpp"
#include "AppState.hpp"
#include <Huenicorn/Channel.hpp>
#include <string>

namespace Huenicorn::Gtk {

void drawScreenPartition(GtkDrawingArea* area, cairo_t* cr, 
                         int width, int height, gpointer user_data) {
    (void)area; (void)user_data;
    
    auto* state = getState();
    
    // Background
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.18);
    cairo_paint(cr);
    
    // Grid
    cairo_set_source_rgba(cr, 0.3, 0.3, 0.4, 0.5);
    cairo_set_line_width(cr, 1.0);
    for (int i = 1; i < 4; i++) {
        cairo_move_to(cr, width * i / 4.0, 0);
        cairo_line_to(cr, width * i / 4.0, height);
        cairo_move_to(cr, 0, height * i / 4.0);
        cairo_line_to(cr, width, height * i / 4.0);
    }
    cairo_stroke(cr);
    
    if (!state || !state->core->isInitialized()) return;
    
    static const double colors[][3] = {
        {0.3, 0.69, 0.31}, {0.13, 0.59, 0.95}, {1.0, 0.6, 0.0},
        {0.61, 0.15, 0.69}, {0.96, 0.26, 0.21}, {0.0, 0.74, 0.83}
    };
    
    const auto& channels = state->core->channels();
    for (const auto& [channelId, channel] : channels) {
        if (channel.state() != Channel::State::Active) continue;
        
        const auto& uvs = channel.uvs();
        double x = uvs.min.x * width, y = uvs.min.y * height;
        double w = (uvs.max.x - uvs.min.x) * width;
        double h = (uvs.max.y - uvs.min.y) * height;
        
        int cIdx = channelId % 6;
        cairo_set_source_rgba(cr, colors[cIdx][0], colors[cIdx][1], colors[cIdx][2], 0.7);
        cairo_rectangle(cr, x, y, w, h);
        cairo_fill(cr);
        
        cairo_set_source_rgba(cr, colors[cIdx][0], colors[cIdx][1], colors[cIdx][2], 1.0);
        cairo_set_line_width(cr, 2.0);
        cairo_rectangle(cr, x, y, w, h);
        cairo_stroke(cr);
        
        // Label
        std::string deviceNames;
        for (const auto& device : channel.devices()) {
            if (!deviceNames.empty()) deviceNames += ", ";
            deviceNames += device.name;
        }
        std::string label = "Ch" + std::to_string(channelId) + ": [" + deviceNames + "]";
        
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 11);
        cairo_move_to(cr, x + 5, y + 15);
        cairo_show_text(cr, label.c_str());
        
        // Handles
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        for (auto [hx, hy] : {std::pair{x, y}, {x+w, y}, {x, y+h}, {x+w, y+h}}) {
            cairo_arc(cr, hx, hy, 5, 0, 2 * G_PI);
            cairo_fill(cr);
        }
    }
}

} // namespace Huenicorn::Gtk
