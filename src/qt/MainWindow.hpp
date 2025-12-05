#pragma once

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QListWidget>
#include <QGroupBox>
#include <QTimer>
#include <thread>

namespace Huenicorn
{
    class HuenicornCore;

    namespace Qt
    {
        class ScreenPartitionWidget;
        class ChannelListWidget;

        /**
         * @brief Main window for Huenicorn Qt application
         */
        class MainWindow : public QMainWindow
        {
            Q_OBJECT

        public:
            explicit MainWindow(HuenicornCore* core, QWidget* parent = nullptr);
            ~MainWindow() override;

            void refreshChannels();
            void refreshEntertainmentConfigurations();

        signals:
            void streamingStarted();
            void streamingStopped();

        public slots:
            void onStartStreaming();
            void onStopStreaming();
            void onSaveProfile();
            void onEntertainmentConfigChanged(int index);
            void onChannelActivated(int channelId, bool active);
            void onChannelUVChanged(int channelId);
            void onChannelGammaChanged(int channelId, float gamma);

        protected:
            void closeEvent(QCloseEvent* event) override;

        private:
            void setupUI();
            void setupConnections();
            void updateStreamingState();

            HuenicornCore* m_core;
            bool m_isStreaming{false};

            // UI Elements
            QWidget* m_centralWidget;
            QVBoxLayout* m_mainLayout;
            
            // Entertainment configuration
            QComboBox* m_entertainmentConfigCombo;
            
            // Screen partitioning
            ScreenPartitionWidget* m_screenPartition;
            
            // Channels
            ChannelListWidget* m_channelList;
            
            // Controls
            QPushButton* m_startButton;
            QPushButton* m_stopButton;
            QPushButton* m_saveButton;
            
            // Status
            QLabel* m_statusLabel;
            
            // Streaming thread
            std::thread m_streamingThread;
        };
    }
}
