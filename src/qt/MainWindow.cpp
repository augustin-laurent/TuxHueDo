#include "MainWindow.hpp"
#include "ScreenPartitionWidget.hpp"
#include "ChannelListWidget.hpp"
#include "SettingsDialog.hpp"

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QCloseEvent>
#include <QMessageBox>
#include <QSplitter>

#include <Huenicorn/HuenicornCore.hpp>
#include <Huenicorn/Logger.hpp>

namespace Huenicorn::Qt
{
    MainWindow::MainWindow(HuenicornCore* core, QWidget* parent)
        : QMainWindow(parent)
        , m_core(core)
    {
        setWindowTitle("Huenicorn Light Manager");
        setMinimumSize(900, 600);
        
        setupUI();
        setupConnections();
        updateStreamingState();
        
        // Initial refresh only if core is initialized
        if (m_core->isInitialized()) {
            refreshEntertainmentConfigurations();
            refreshChannels();
        }
    }

    MainWindow::~MainWindow()
    {
        if (m_streamingThread.joinable()) {
            m_core->stop();
            m_streamingThread.join();
        }
    }

    void MainWindow::setupUI()
    {
        m_centralWidget = new QWidget(this);
        setCentralWidget(m_centralWidget);
        
        m_mainLayout = new QVBoxLayout(m_centralWidget);
        m_mainLayout->setSpacing(10);
        m_mainLayout->setContentsMargins(10, 10, 10, 10);

        // Menu bar
        auto* fileMenu = menuBar()->addMenu(tr("&File"));
        auto* saveAction = fileMenu->addAction(tr("&Save Profile"));
        saveAction->setShortcut(QKeySequence::Save);
        connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveProfile);
        
        fileMenu->addSeparator();
        auto* quitAction = fileMenu->addAction(tr("&Quit"));
        quitAction->setShortcut(QKeySequence::Quit);
        connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

        auto* settingsMenu = menuBar()->addMenu(tr("&Settings"));
        auto* prefsAction = settingsMenu->addAction(tr("&Preferences..."));
        connect(prefsAction, &QAction::triggered, this, [this]() {
            SettingsDialog dialog(m_core, this);
            dialog.exec();
        });

        // Entertainment configuration selector
        auto* configGroup = new QGroupBox(tr("Entertainment Configuration"), this);
        auto* configLayout = new QHBoxLayout(configGroup);
        m_entertainmentConfigCombo = new QComboBox(this);
        configLayout->addWidget(new QLabel(tr("Configuration:"), this));
        configLayout->addWidget(m_entertainmentConfigCombo, 1);
        m_mainLayout->addWidget(configGroup);

        // Main content with splitter
        auto* splitter = new QSplitter(::Qt::Horizontal, this);
        
        // Screen partition widget (left)
        auto* screenGroup = new QGroupBox(tr("Screen Partitioning"), this);
        auto* screenLayout = new QVBoxLayout(screenGroup);
        m_screenPartition = new ScreenPartitionWidget(m_core, this);
        screenLayout->addWidget(m_screenPartition);
        splitter->addWidget(screenGroup);

        // Channel list (right)
        auto* channelGroup = new QGroupBox(tr("Channels"), this);
        auto* channelLayout = new QVBoxLayout(channelGroup);
        m_channelList = new ChannelListWidget(m_core, this);
        channelLayout->addWidget(m_channelList);
        splitter->addWidget(channelGroup);
        
        splitter->setStretchFactor(0, 2);
        splitter->setStretchFactor(1, 1);
        m_mainLayout->addWidget(splitter, 1);

        // Control buttons
        auto* controlLayout = new QHBoxLayout();
        m_startButton = new QPushButton(tr("Start Streaming"), this);
        m_startButton->setIcon(QIcon::fromTheme("media-playback-start"));
        m_stopButton = new QPushButton(tr("Stop Streaming"), this);
        m_stopButton->setIcon(QIcon::fromTheme("media-playback-stop"));
        m_saveButton = new QPushButton(tr("Save Profile"), this);
        m_saveButton->setIcon(QIcon::fromTheme("document-save"));
        
        controlLayout->addStretch();
        controlLayout->addWidget(m_startButton);
        controlLayout->addWidget(m_stopButton);
        controlLayout->addWidget(m_saveButton);
        controlLayout->addStretch();
        m_mainLayout->addLayout(controlLayout);

        // Status bar
        m_statusLabel = new QLabel(tr("Ready"), this);
        statusBar()->addWidget(m_statusLabel);
    }

    void MainWindow::setupConnections()
    {
        connect(m_entertainmentConfigCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &MainWindow::onEntertainmentConfigChanged);
        
        connect(m_startButton, &QPushButton::clicked, this, &MainWindow::onStartStreaming);
        connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::onStopStreaming);
        connect(m_saveButton, &QPushButton::clicked, this, &MainWindow::onSaveProfile);
        
        connect(m_channelList, &ChannelListWidget::channelActivated,
                this, &MainWindow::onChannelActivated);
        connect(m_channelList, &ChannelListWidget::channelGammaChanged,
                this, &MainWindow::onChannelGammaChanged);
        
        connect(m_screenPartition, &ScreenPartitionWidget::channelUVChanged,
                this, &MainWindow::onChannelUVChanged);
    }

    void MainWindow::refreshEntertainmentConfigurations()
    {
        if (!m_core->isInitialized()) return;
        
        m_entertainmentConfigCombo->blockSignals(true);
        m_entertainmentConfigCombo->clear();
        
        const auto& configs = m_core->entertainmentConfigurations();
        auto currentId = m_core->currentEntertainmentConfigurationId();
        int currentIndex = 0;
        int i = 0;
        
        for (const auto& [id, config] : configs) {
            m_entertainmentConfigCombo->addItem(
                QString::fromStdString(config.name),
                QString::fromStdString(id)
            );
            if (currentId.has_value() && id == currentId.value()) {
                currentIndex = i;
            }
            i++;
        }
        
        m_entertainmentConfigCombo->setCurrentIndex(currentIndex);
        m_entertainmentConfigCombo->blockSignals(false);
    }

    void MainWindow::refreshChannels()
    {
        if (!m_core->isInitialized()) return;
        
        m_channelList->refresh();
        m_screenPartition->refresh();
    }

    void MainWindow::onStartStreaming()
    {
        if (m_isStreaming) return;
        
        m_isStreaming = true;
        updateStreamingState();
        
        m_streamingThread = std::thread([this]() {
            m_core->start();
            m_isStreaming = false;
            QMetaObject::invokeMethod(this, &MainWindow::updateStreamingState);
        });
    }

    void MainWindow::onStopStreaming()
    {
        if (!m_isStreaming) return;
        
        m_core->stop();
        if (m_streamingThread.joinable()) {
            m_streamingThread.join();
        }
        m_isStreaming = false;
        updateStreamingState();
    }

    void MainWindow::onSaveProfile()
    {
        m_core->saveProfile();
        m_statusLabel->setText(tr("Profile saved"));
    }

    void MainWindow::onEntertainmentConfigChanged(int index)
    {
        if (index < 0) return;
        
        QString configId = m_entertainmentConfigCombo->currentData().toString();
        m_core->setEntertainmentConfiguration(configId.toStdString());
        refreshChannels();
    }

    void MainWindow::onChannelActivated(int channelId, bool active)
    {
        m_core->setChannelActivity(static_cast<uint8_t>(channelId), active);
        m_screenPartition->refresh();
    }

    void MainWindow::onChannelUVChanged(int channelId)
    {
        m_channelList->refresh();
    }

    void MainWindow::onChannelGammaChanged(int channelId, float gamma)
    {
        m_core->setChannelGammaFactor(static_cast<uint8_t>(channelId), gamma);
    }

    void MainWindow::updateStreamingState()
    {
        m_startButton->setEnabled(!m_isStreaming);
        m_stopButton->setEnabled(m_isStreaming);
        m_entertainmentConfigCombo->setEnabled(!m_isStreaming);
        
        if (m_isStreaming) {
            m_statusLabel->setText(tr("Streaming..."));
        } else {
            m_statusLabel->setText(tr("Ready"));
        }
    }

    void MainWindow::closeEvent(QCloseEvent* event)
    {
        // Minimize to tray instead of closing
        hide();
        event->ignore();
    }
}
