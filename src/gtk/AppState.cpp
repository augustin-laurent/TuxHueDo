// GTK4 Application State Implementation
#include "AppState.hpp"

namespace Huenicorn::Gtk {

static AppState* g_state = nullptr;

AppState* getState() {
    return g_state;
}

void setState(AppState* state) {
    g_state = state;
}

void cleanupState() {
    if (g_state) {
        if (g_state->is_streaming) {
            g_state->core->stop();
            if (g_state->streaming_thread.joinable()) {
                g_state->streaming_thread.join();
            }
        }
        delete g_state;
        g_state = nullptr;
    }
}

} // namespace Huenicorn::Gtk
