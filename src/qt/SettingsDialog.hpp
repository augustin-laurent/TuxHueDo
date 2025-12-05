#pragma once

#include <QDialog>
#include <QComboBox>
#include <QSlider>
#include <QSpinBox>
#include <QLabel>

namespace Huenicorn
{
    class HuenicornCore;

    namespace Qt
    {
        /**
         * @brief Settings dialog for advanced Huenicorn options
         */
        class SettingsDialog : public QDialog
        {
            Q_OBJECT

        public:
            explicit SettingsDialog(HuenicornCore* core, QWidget* parent = nullptr);

        private slots:
            void onSubsampleChanged(int value);
            void onRefreshRateChanged(int value);
            void onInterpolationChanged(int index);

        private:
            void setupUI();
            void loadSettings();

            HuenicornCore* m_core;

            QSpinBox* m_subsampleSpinBox;
            QSlider* m_subsampleSlider;
            QSpinBox* m_refreshRateSpinBox;
            QSlider* m_refreshRateSlider;
            QComboBox* m_interpolationCombo;
            QLabel* m_displayResLabel;
        };
    }
}
