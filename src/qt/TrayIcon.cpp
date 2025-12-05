#include "TrayIcon.hpp"
#include "MainWindow.hpp"
#include <Huenicorn/HuenicornCore.hpp>
#include <QApplication>

namespace Huenicorn::Qt
{
    TrayIcon::TrayIcon(HuenicornCore* core, MainWindow* mainWindow, QObject* parent)
        : QSystemTrayIcon(parent)
        , m_core(core)
        , m_mainWindow(mainWindow)
    {
        // Set icon
        setIcon(QIcon::fromTheme("preferences-desktop-color", 
                QIcon(":/icons/huenicorn.png")));
        setToolTip("Huenicorn");
        
        setupMenu();
        
        connect(this, &QSystemTrayIcon::activated, 
                this, &TrayIcon::onActivated);
        
        // Connect to main window streaming signals
        connect(m_mainWindow, &MainWindow::streamingStarted, this, [this]() {
            updateStreamingState(true);
        });
        connect(m_mainWindow, &MainWindow::streamingStopped, this, [this]() {
            updateStreamingState(false);
        });
    }

    void TrayIcon::setupMenu()
    {
        m_menu = new QMenu();
        
        auto* showAction = m_menu->addAction(QIcon::fromTheme("window"), tr("Show Window"));
        connect(showAction, &QAction::triggered, this, &TrayIcon::onShowWindow);
        
        m_menu->addSeparator();
        
        m_startAction = m_menu->addAction(QIcon::fromTheme("media-playback-start"), tr("Start Streaming"));
        connect(m_startAction, &QAction::triggered, this, &TrayIcon::onStartStreaming);
        
        m_stopAction = m_menu->addAction(QIcon::fromTheme("media-playback-stop"), tr("Stop Streaming"));
        m_stopAction->setEnabled(false);
        connect(m_stopAction, &QAction::triggered, this, &TrayIcon::onStopStreaming);
        
        m_menu->addSeparator();
        
        auto* quitAction = m_menu->addAction(QIcon::fromTheme("application-exit"), tr("Quit"));
        connect(quitAction, &QAction::triggered, this, &TrayIcon::onQuit);
        
        setContextMenu(m_menu);
    }

    void TrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
    {
        if (reason == QSystemTrayIcon::Trigger || 
            reason == QSystemTrayIcon::DoubleClick) {
            onShowWindow();
        }
    }

    void TrayIcon::onShowWindow()
    {
        m_mainWindow->show();
        m_mainWindow->raise();
        m_mainWindow->activateWindow();
    }

    void TrayIcon::onStartStreaming()
    {
        m_mainWindow->onStartStreaming();
    }

    void TrayIcon::onStopStreaming()
    {
        m_mainWindow->onStopStreaming();
    }

    void TrayIcon::onQuit()
    {
        m_mainWindow->onStopStreaming();
        QApplication::quit();
    }

    void TrayIcon::updateStreamingState(bool streaming)
    {
        m_startAction->setEnabled(!streaming);
        m_stopAction->setEnabled(streaming);
        
        if (streaming) {
            setToolTip("Huenicorn - Streaming");
        } else {
            setToolTip("Huenicorn");
        }
    }
}
