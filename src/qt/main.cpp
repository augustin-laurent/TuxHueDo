#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include <QStandardPaths>

#include "MainWindow.hpp"
#include "TrayIcon.hpp"
#include <Huenicorn/HuenicornCore.hpp>
#include <Huenicorn/Version.hpp>
#include <Huenicorn/Logger.hpp>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Huenicorn");
    app.setApplicationVersion(QString::fromStdString(Huenicorn::Version));
    app.setOrganizationName("Huenicorn");
    app.setQuitOnLastWindowClosed(false);

    // Determine config directory
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/huenicorn";
    QDir().mkpath(configPath);

    try {
        // Initialize Huenicorn core
        auto core = std::make_unique<Huenicorn::HuenicornCore>(
            Huenicorn::Version,
            configPath.toStdString()
        );

        // If not configured, try to auto-configure
        if (!core->isConfigured()) {
            auto bridgeResult = core->autodetectedBridge();
            
            if (bridgeResult.contains("succeeded") && bridgeResult["succeeded"].get<bool>()) {
                if (bridgeResult.contains("bridges") && !bridgeResult["bridges"].empty()) {
                    std::string bridgeAddress = "http://" + bridgeResult["bridges"][0]["internalipaddress"].get<std::string>();
                    core->validateBridgeAddress(bridgeAddress);
                    
                    QMessageBox::information(nullptr, "Huenicorn Setup",
                        QString("Bridge found at %1\n\nPlease press the link button on your Hue bridge, then click OK.")
                        .arg(QString::fromStdString(bridgeAddress)));
                    
                    auto registerResult = core->registerNewUser();
                    if (!registerResult["succeeded"].get<bool>()) {
                        Huenicorn::Logger::error("Failed to register: ", registerResult["error"].get<std::string>());
                        QMessageBox::critical(nullptr, "Error", "Failed to register with bridge");
                        return 1;
                    }
                }
            } else {
                Huenicorn::Logger::error("No Hue bridge found");
                QMessageBox::critical(nullptr, "Error", "No Hue bridge found. Please check your network.");
                return 1;
            }
        }

        // Initialize the core
        if (!core->initialize()) {
            Huenicorn::Logger::error("Core initialization failed");
            QMessageBox::critical(nullptr, "Error", "Failed to initialize Huenicorn");
            return 1;
        }

        // Create main window
        Huenicorn::Qt::MainWindow mainWindow(core.get());
        
        // Create tray icon
        Huenicorn::Qt::TrayIcon trayIcon(core.get(), &mainWindow);
        trayIcon.show();

        // Show main window
        mainWindow.show();

        return app.exec();
    }
    catch (const std::exception& e) {
        Huenicorn::Logger::error("Fatal error: ", e.what());
        QMessageBox::critical(nullptr, "Huenicorn Error", 
            QString("Failed to initialize: %1").arg(e.what()));
        return 1;
    }
}
