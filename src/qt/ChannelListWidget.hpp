#pragma once

#include <QWidget>
#include <QListWidget>
#include <QSlider>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>

namespace Huenicorn
{
    class HuenicornCore;

    namespace Qt
    {
        /**
         * @brief Widget for managing channel list with activation and gamma controls
         */
        class ChannelListWidget : public QWidget
        {
            Q_OBJECT

        public:
            explicit ChannelListWidget(HuenicornCore* core, QWidget* parent = nullptr);

            void refresh();

        signals:
            void channelActivated(int channelId, bool active);
            void channelGammaChanged(int channelId, float gamma);

        private:
            struct ChannelItem {
                int channelId;
                QWidget* widget;
                QCheckBox* activeCheck;
                QSlider* gammaSlider;
                QLabel* gammaLabel;
            };

            void createChannelItem(int channelId, const QString& name, bool active, float gamma);
            void clearItems();

            HuenicornCore* m_core;
            QVBoxLayout* m_layout;
            QVector<ChannelItem> m_items;
        };
    }
}
