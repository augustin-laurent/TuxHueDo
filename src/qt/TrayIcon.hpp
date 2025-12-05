#pragma once

#include <QSystemTrayIcon>
#include <QMenu>

namespace Huenicorn
{
    class HuenicornCore;

    namespace Qt
    {
        class MainWindow;

        /**
         * @brief System tray icon with context menu
         */
        class TrayIcon : public QSystemTrayIcon
        {
            Q_OBJECT

        public:
            TrayIcon(HuenicornCore* core, MainWindow* mainWindow, QObject* parent = nullptr);

        private slots:
            void onActivated(QSystemTrayIcon::ActivationReason reason);
            void onShowWindow();
            void onStartStreaming();
            void onStopStreaming();
            void onQuit();

        private:
            void setupMenu();
            void updateStreamingState(bool streaming);

            HuenicornCore* m_core;
            MainWindow* m_mainWindow;
            QMenu* m_menu;
            QAction* m_startAction;
            QAction* m_stopAction;
        };
    }
}
