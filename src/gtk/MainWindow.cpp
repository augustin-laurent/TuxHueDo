// Main Window Implementation
#include "MainWindow.hpp"
#include "AppState.hpp"
#include "ScreenPartitionWidget.hpp"
#include "SettingsDialog.hpp"

#include <Huenicorn/HuenicornCore.hpp>
#include <Huenicorn/Version.hpp>
#include <Huenicorn/Logger.hpp>
#include <Huenicorn/Channel.hpp>

namespace Huenicorn::Gtk {

// Forward declarations
static void onChannelToggled(GtkCheckButton* button, gpointer user_data);
static void onConfigChanged(GtkDropDown* dropdown, GParamSpec* pspec, gpointer user_data);
static void onStartStreaming(GtkWidget* button, gpointer user_data);
static void onStopStreaming(GtkWidget* button, gpointer user_data);
static void onSaveProfile(GtkWidget* button, gpointer user_data);

void refreshChannels() {
    auto* state = getState();
    if (!state || !state->core->isInitialized()) return;
    
    GtkWidget* child;
    while ((child = gtk_widget_get_first_child(state->channel_box)))
        gtk_box_remove(GTK_BOX(state->channel_box), child);
    
    const auto& channels = state->core->channels();
    for (const auto& [channelId, channel] : channels) {
        GtkWidget* row = adw_action_row_new();
        
        std::string deviceNames;
        for (const auto& device : channel.devices()) {
            if (!deviceNames.empty()) deviceNames += ", ";
            deviceNames += device.name;
        }
        std::string title = "Channel " + std::to_string(channelId) + ": [" + deviceNames + "]";
        adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), title.c_str());
        
        GtkWidget* check = gtk_check_button_new();
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), 
            channel.state() == Channel::State::Active);
        g_object_set_data(G_OBJECT(check), "channel_id", GINT_TO_POINTER(channelId));
        g_signal_connect(check, "toggled", G_CALLBACK(onChannelToggled), nullptr);
        adw_action_row_add_prefix(ADW_ACTION_ROW(row), check);
        
        gtk_box_append(GTK_BOX(state->channel_box), row);
    }
    gtk_widget_queue_draw(state->screen_drawing);
}

void updateStreamingState() {
    auto* state = getState();
    gtk_widget_set_sensitive(state->start_button, !state->is_streaming);
    gtk_widget_set_sensitive(state->stop_button, state->is_streaming);
    gtk_widget_set_sensitive(state->config_dropdown, !state->is_streaming);
    gtk_label_set_text(GTK_LABEL(state->status_label), 
                       state->is_streaming ? "Streaming..." : "Ready");
}

static void onChannelToggled(GtkCheckButton* button, gpointer user_data) {
    (void)user_data;
    auto* state = getState();
    int channelId = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "channel_id"));
    bool active = gtk_check_button_get_active(button);
    state->core->setChannelActivity(static_cast<uint8_t>(channelId), active);
    gtk_widget_queue_draw(state->screen_drawing);
}

static void onConfigChanged(GtkDropDown* dropdown, GParamSpec* pspec, gpointer user_data) {
    (void)pspec; (void)user_data;
    auto* state = getState();
    guint selected = gtk_drop_down_get_selected(dropdown);
    const auto& configs = state->core->entertainmentConfigurations();
    
    guint idx = 0;
    for (const auto& [id, config] : configs) {
        if (idx == selected) {
            state->core->setEntertainmentConfiguration(id);
            refreshChannels();
            break;
        }
        idx++;
    }
}

static void onStartStreaming(GtkWidget* button, gpointer user_data) {
    (void)button; (void)user_data;
    auto* state = getState();
    if (state->is_streaming) return;
    
    state->is_streaming = true;
    updateStreamingState();
    
    state->streaming_thread = std::thread([]() {
        auto* s = getState();
        s->core->start();
        s->is_streaming = false;
        g_idle_add([](gpointer) -> gboolean {
            updateStreamingState();
            return G_SOURCE_REMOVE;
        }, nullptr);
    });
}

static void onStopStreaming(GtkWidget* button, gpointer user_data) {
    (void)button; (void)user_data;
    auto* state = getState();
    if (!state->is_streaming) return;
    
    state->core->stop();
    if (state->streaming_thread.joinable()) {
        state->streaming_thread.join();
    }
    state->is_streaming = false;
    updateStreamingState();
}

static void onSaveProfile(GtkWidget* button, gpointer user_data) {
    (void)button; (void)user_data;
    auto* state = getState();
    state->core->saveProfile();
    gtk_label_set_text(GTK_LABEL(state->status_label), "Profile saved");
}

void setupMainWindow(GtkApplication* app) {
    auto* state = getState();
    
    // Initialize core
    const char* config_dir = g_get_user_config_dir();
    std::string config_path = std::string(config_dir) + "/huenicorn";
    g_mkdir_with_parents(config_path.c_str(), 0755);
    
    try {
        state->core = std::make_unique<HuenicornCore>(Version, config_path);
        
        if (!state->core->isConfigured()) {
            auto result = state->core->autodetectedBridge();
            if (result.contains("succeeded") && result["succeeded"].get<bool>() &&
                result.contains("bridges") && !result["bridges"].empty()) {
                std::string addr = "http://" + result["bridges"][0]["internalipaddress"].get<std::string>();
                state->core->validateBridgeAddress(addr);
                
                GtkAlertDialog* dialog = gtk_alert_dialog_new(
                    "Bridge found at %s\n\nPress the link button, then click OK.", addr.c_str());
                gtk_alert_dialog_show(dialog, nullptr);
                g_object_unref(dialog);
                
                state->core->registerNewUser();
            }
        }
        
        if (!state->core->initialize()) {
            Logger::error("Failed to initialize");
            return;
        }
    } catch (const std::exception& e) {
        Logger::error("Error: ", e.what());
        return;
    }
    
    // Build UI
    state->main_window = adw_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(state->main_window), "Huenicorn");
    gtk_window_set_default_size(GTK_WINDOW(state->main_window), 900, 600);
    
    GtkWidget* header = adw_header_bar_new();
    
    // Settings button
    GtkWidget* settings_btn = gtk_button_new_from_icon_name("preferences-system-symbolic");
    gtk_widget_set_tooltip_text(settings_btn, "Settings");
    g_signal_connect(settings_btn, "clicked", G_CALLBACK(+[](GtkButton*, gpointer) {
        showSettingsDialog();
    }), nullptr);
    adw_header_bar_pack_end(ADW_HEADER_BAR(header), settings_btn);
    
    GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    AdwToolbarView* toolbar = ADW_TOOLBAR_VIEW(adw_toolbar_view_new());
    adw_toolbar_view_add_top_bar(toolbar, header);
    adw_toolbar_view_set_content(toolbar, main_box);
    
    GtkWidget* content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start(content, 12);
    gtk_widget_set_margin_end(content, 12);
    gtk_widget_set_margin_top(content, 12);
    gtk_widget_set_margin_bottom(content, 12);
    gtk_box_append(GTK_BOX(main_box), content);
    
    // Config dropdown
    GtkStringList* config_list = gtk_string_list_new(nullptr);
    for (const auto& [id, config] : state->core->entertainmentConfigurations())
        gtk_string_list_append(config_list, config.name.c_str());
    
    state->config_dropdown = gtk_drop_down_new(G_LIST_MODEL(config_list), nullptr);
    GtkWidget* config_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_append(GTK_BOX(config_box), gtk_label_new("Configuration:"));
    gtk_box_append(GTK_BOX(config_box), state->config_dropdown);
    gtk_widget_set_hexpand(state->config_dropdown, TRUE);
    gtk_box_append(GTK_BOX(content), config_box);
    g_signal_connect(state->config_dropdown, "notify::selected", G_CALLBACK(onConfigChanged), nullptr);
    
    // Paned view
    GtkWidget* paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_vexpand(paned, TRUE);
    gtk_paned_set_position(GTK_PANED(paned), 550);
    
    GtkWidget* screen_frame = gtk_frame_new("Screen Partitioning");
    gtk_widget_set_margin_end(screen_frame, 6);
    state->screen_drawing = gtk_drawing_area_new();
    gtk_widget_set_hexpand(state->screen_drawing, TRUE);
    gtk_widget_set_vexpand(state->screen_drawing, TRUE);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(state->screen_drawing),
                                    drawScreenPartition, nullptr, nullptr);
    gtk_frame_set_child(GTK_FRAME(screen_frame), state->screen_drawing);
    gtk_paned_set_start_child(GTK_PANED(paned), screen_frame);
    
    GtkWidget* channels_frame = gtk_frame_new("Channels");
    gtk_widget_set_margin_start(channels_frame, 6);
    GtkWidget* scroll = gtk_scrolled_window_new();
    state->channel_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_margin_start(state->channel_box, 6);
    gtk_widget_set_margin_end(state->channel_box, 6);
    gtk_widget_set_margin_top(state->channel_box, 6);
    gtk_widget_set_margin_bottom(state->channel_box, 6);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), state->channel_box);
    gtk_frame_set_child(GTK_FRAME(channels_frame), scroll);
    gtk_paned_set_end_child(GTK_PANED(paned), channels_frame);
    
    gtk_box_append(GTK_BOX(content), paned);
    
    // Buttons
    GtkWidget* btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_halign(btn_box, GTK_ALIGN_CENTER);
    
    state->start_button = gtk_button_new_with_label("Start Streaming");
    gtk_widget_add_css_class(state->start_button, "suggested-action");
    g_signal_connect(state->start_button, "clicked", G_CALLBACK(onStartStreaming), nullptr);
    
    state->stop_button = gtk_button_new_with_label("Stop Streaming");
    gtk_widget_add_css_class(state->stop_button, "destructive-action");
    gtk_widget_set_sensitive(state->stop_button, FALSE);
    g_signal_connect(state->stop_button, "clicked", G_CALLBACK(onStopStreaming), nullptr);
    
    GtkWidget* save_btn = gtk_button_new_with_label("Save Profile");
    g_signal_connect(save_btn, "clicked", G_CALLBACK(onSaveProfile), nullptr);
    
    gtk_box_append(GTK_BOX(btn_box), state->start_button);
    gtk_box_append(GTK_BOX(btn_box), state->stop_button);
    gtk_box_append(GTK_BOX(btn_box), save_btn);
    gtk_box_append(GTK_BOX(content), btn_box);
    
    state->status_label = gtk_label_new("Ready");
    gtk_widget_add_css_class(state->status_label, "dim-label");
    gtk_box_append(GTK_BOX(content), state->status_label);
    
    adw_application_window_set_content(ADW_APPLICATION_WINDOW(state->main_window), GTK_WIDGET(toolbar));
    
    refreshChannels();
    gtk_window_present(GTK_WINDOW(state->main_window));
}

} // namespace Huenicorn::Gtk
