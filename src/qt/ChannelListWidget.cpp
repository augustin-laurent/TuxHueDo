#include "ChannelListWidget.hpp"
#include <Huenicorn/HuenicornCore.hpp>
#include <Huenicorn/Channel.hpp>
#include <QScrollArea>
#include <QFrame>

namespace Huenicorn::Qt
{
    ChannelListWidget::ChannelListWidget(HuenicornCore* core, QWidget* parent)
        : QWidget(parent)
        , m_core(core)
    {
        auto* scrollArea = new QScrollArea(this);
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameShape(QFrame::NoFrame);
        
        auto* container = new QWidget();
        m_layout = new QVBoxLayout(container);
        m_layout->setSpacing(5);
        m_layout->setContentsMargins(5, 5, 5, 5);
        m_layout->addStretch();
        
        scrollArea->setWidget(container);
        
        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->addWidget(scrollArea);
    }

    void ChannelListWidget::refresh()
    {
        if (!m_core->isInitialized()) return;
        
        clearItems();
        
        const auto& channels = m_core->channels();
        
        for (const auto& [channelId, channel] : channels) {
            QStringList deviceNames;
            for (const auto& device : channel.devices()) {
                deviceNames << QString::fromStdString(device.name);
            }
            QString name = QString("Channel %1: [%2]").arg(channelId).arg(deviceNames.join(", "));
            bool active = (channel.state() == Channel::State::Active);
            float gamma = channel.gammaFactor();
            
            createChannelItem(channelId, name, active, gamma);
        }
    }

    void ChannelListWidget::createChannelItem(int channelId, const QString& name, bool active, float gamma)
    {
        auto* itemWidget = new QWidget();
        itemWidget->setStyleSheet("QWidget { background-color: #2d2d44; border-radius: 5px; padding: 5px; }");
        
        auto* layout = new QVBoxLayout(itemWidget);
        layout->setSpacing(5);
        
        // Active checkbox with name
        auto* activeCheck = new QCheckBox(name);
        activeCheck->setChecked(active);
        activeCheck->setStyleSheet("QCheckBox { color: white; font-weight: bold; }");
        layout->addWidget(activeCheck);
        
        // Gamma slider
        auto* gammaLayout = new QHBoxLayout();
        auto* gammaLabelTitle = new QLabel(tr("Gamma:"));
        gammaLabelTitle->setStyleSheet("color: #aaa;");
        auto* gammaSlider = new QSlider(::Qt::Horizontal);
        gammaSlider->setRange(-100, 100);
        gammaSlider->setValue(static_cast<int>(gamma * 100));
        gammaSlider->setEnabled(active);
        
        auto* gammaLabel = new QLabel(QString::number(gamma, 'f', 2));
        gammaLabel->setStyleSheet("color: white; min-width: 40px;");
        
        gammaLayout->addWidget(gammaLabelTitle);
        gammaLayout->addWidget(gammaSlider, 1);
        gammaLayout->addWidget(gammaLabel);
        layout->addLayout(gammaLayout);
        
        // Store item
        ChannelItem item{channelId, itemWidget, activeCheck, gammaSlider, gammaLabel};
        m_items.append(item);
        
        // Insert before stretch
        m_layout->insertWidget(m_layout->count() - 1, itemWidget);
        
        // Connections
        connect(activeCheck, &QCheckBox::toggled, this, [this, channelId, gammaSlider](bool checked) {
            gammaSlider->setEnabled(checked);
            emit channelActivated(channelId, checked);
        });
        
        connect(gammaSlider, &QSlider::valueChanged, this, [this, channelId, gammaLabel](int value) {
            float gamma = value / 100.0f;
            gammaLabel->setText(QString::number(gamma, 'f', 2));
            emit channelGammaChanged(channelId, gamma);
        });
    }

    void ChannelListWidget::clearItems()
    {
        for (auto& item : m_items) {
            m_layout->removeWidget(item.widget);
            delete item.widget;
        }
        m_items.clear();
    }
}
