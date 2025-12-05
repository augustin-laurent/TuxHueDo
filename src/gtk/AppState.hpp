// GTK4 Application State and Core
#pragma once

#include <adwaita.h>
#include <thread>
#include <memory>

#include <Huenicorn/HuenicornCore.hpp>

namespace Huenicorn::Gtk {

struct AppState {
    std::unique_ptr<HuenicornCore> core;
    GtkWidget* main_window = nullptr;
    GtkWidget* config_dropdown = nullptr;
    GtkWidget* channel_box = nullptr;
    GtkWidget* screen_drawing = nullptr;
    GtkWidget* start_button = nullptr;
    GtkWidget* stop_button = nullptr;
    GtkWidget* status_label = nullptr;
    std::thread streaming_thread;
    bool is_streaming = false;
};

// Global state accessor
AppState* getState();
void setState(AppState* state);
void cleanupState();

// UI refresh functions
void refreshChannels();
void updateStreamingState();

} // namespace Huenicorn::Gtk
