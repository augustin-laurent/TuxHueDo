// Settings Dialog Implementation
#include "SettingsDialog.hpp"
#include "AppState.hpp"
#include <Huenicorn/Interpolation.hpp>

namespace Huenicorn::Gtk {

void showSettingsDialog() {
    auto* state = getState();
    if (!state) return;
    
    AdwDialog* dialog = ADW_DIALOG(adw_dialog_new());
    adw_dialog_set_title(dialog, "Settings");
    adw_dialog_set_content_width(dialog, 400);
    adw_dialog_set_content_height(dialog, 350);
    
    GtkWidget* toolbar_view = adw_toolbar_view_new();
    
    // Header
    GtkWidget* header = adw_header_bar_new();
    adw_toolbar_view_add_top_bar(ADW_TOOLBAR_VIEW(toolbar_view), header);
    
    // Content
    GtkWidget* prefs_page = adw_preferences_page_new();
    
    // Display Settings Group
    GtkWidget* display_group = adw_preferences_group_new();
    adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(display_group), "Display Settings");
    
    // Subsample Width
    GtkWidget* subsample_row = adw_spin_row_new_with_range(5, 100, 1);
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(subsample_row), "Subsample Width");
    adw_action_row_set_subtitle(ADW_ACTION_ROW(subsample_row), "Width in pixels for color sampling");
    adw_spin_row_set_value(ADW_SPIN_ROW(subsample_row), state->core->subsampleWidth());
    g_signal_connect(subsample_row, "notify::value", G_CALLBACK(+[](AdwSpinRow* row, GParamSpec*, gpointer) {
        getState()->core->setSubsampleWidth(static_cast<unsigned>(adw_spin_row_get_value(row)));
    }), nullptr);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(display_group), subsample_row);
    
    // Refresh Rate
    GtkWidget* refresh_row = adw_spin_row_new_with_range(1, 144, 1);
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(refresh_row), "Refresh Rate");
    adw_action_row_set_subtitle(ADW_ACTION_ROW(refresh_row), "Target refresh rate in Hz");
    adw_spin_row_set_value(ADW_SPIN_ROW(refresh_row), state->core->refreshRate());
    g_signal_connect(refresh_row, "notify::value", G_CALLBACK(+[](AdwSpinRow* row, GParamSpec*, gpointer) {
        getState()->core->setRefreshRate(static_cast<unsigned>(adw_spin_row_get_value(row)));
    }), nullptr);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(display_group), refresh_row);
    
    adw_preferences_page_add(ADW_PREFERENCES_PAGE(prefs_page), ADW_PREFERENCES_GROUP(display_group));
    
    // Interpolation Group
    GtkWidget* interp_group = adw_preferences_group_new();
    adw_preferences_group_set_title(ADW_PREFERENCES_GROUP(interp_group), "Interpolation");
    
    // Interpolation dropdown
    GtkStringList* interp_list = gtk_string_list_new(nullptr);
    const auto& interpolations = state->core->availableInterpolations();
    int currentIdx = 0, i = 0;
    for (const auto& [name, type] : interpolations) {
        gtk_string_list_append(interp_list, name.c_str());
        if (type == state->core->interpolation()) currentIdx = i;
        i++;
    }
    
    GtkWidget* interp_row = adw_combo_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(interp_row), "Algorithm");
    adw_action_row_set_subtitle(ADW_ACTION_ROW(interp_row), "Color interpolation method");
    adw_combo_row_set_model(ADW_COMBO_ROW(interp_row), G_LIST_MODEL(interp_list));
    adw_combo_row_set_selected(ADW_COMBO_ROW(interp_row), currentIdx);
    g_signal_connect(interp_row, "notify::selected", G_CALLBACK(+[](AdwComboRow* row, GParamSpec*, gpointer) {
        getState()->core->setInterpolation(adw_combo_row_get_selected(row));
    }), nullptr);
    adw_preferences_group_add(ADW_PREFERENCES_GROUP(interp_group), interp_row);
    
    adw_preferences_page_add(ADW_PREFERENCES_PAGE(prefs_page), ADW_PREFERENCES_GROUP(interp_group));
    
    adw_toolbar_view_set_content(ADW_TOOLBAR_VIEW(toolbar_view), prefs_page);
    adw_dialog_set_child(dialog, toolbar_view);
    
    adw_dialog_present(dialog, GTK_WIDGET(state->main_window));
}

} // namespace Huenicorn::Gtk
