#include "SettingsDialog.hpp"
#include <Huenicorn/HuenicornCore.hpp>
#include <Huenicorn/Interpolation.hpp>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QDialogButtonBox>

namespace Huenicorn::Qt
{
    SettingsDialog::SettingsDialog(HuenicornCore* core, QWidget* parent)
        : QDialog(parent)
        , m_core(core)
    {
        setWindowTitle(tr("Preferences"));
        setMinimumWidth(400);
        
        setupUI();
        loadSettings();
    }

    void SettingsDialog::setupUI()
    {
        auto* mainLayout = new QVBoxLayout(this);
        
        // Display info group
        auto* displayGroup = new QGroupBox(tr("Display"), this);
        auto* displayLayout = new QFormLayout(displayGroup);
        
        m_displayResLabel = new QLabel(this);
        displayLayout->addRow(tr("Resolution:"), m_displayResLabel);
        
        mainLayout->addWidget(displayGroup);
        
        // Performance group
        auto* perfGroup = new QGroupBox(tr("Performance"), this);
        auto* perfLayout = new QFormLayout(perfGroup);
        
        // Subsample width
        auto* subsampleLayout = new QHBoxLayout();
        m_subsampleSlider = new QSlider(::Qt::Horizontal, this);
        m_subsampleSpinBox = new QSpinBox(this);
        m_subsampleSpinBox->setSuffix(" px");
        subsampleLayout->addWidget(m_subsampleSlider, 1);
        subsampleLayout->addWidget(m_subsampleSpinBox);
        perfLayout->addRow(tr("Subsample Width:"), subsampleLayout);
        
        // Refresh rate
        auto* refreshLayout = new QHBoxLayout();
        m_refreshRateSlider = new QSlider(::Qt::Horizontal, this);
        m_refreshRateSpinBox = new QSpinBox(this);
        m_refreshRateSpinBox->setSuffix(" Hz");
        refreshLayout->addWidget(m_refreshRateSlider, 1);
        refreshLayout->addWidget(m_refreshRateSpinBox);
        perfLayout->addRow(tr("Refresh Rate:"), refreshLayout);
        
        // Interpolation
        m_interpolationCombo = new QComboBox(this);
        perfLayout->addRow(tr("Interpolation:"), m_interpolationCombo);
        
        mainLayout->addWidget(perfGroup);
        
        // Buttons
        auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
        mainLayout->addWidget(buttonBox);
        
        // Connections
        connect(m_subsampleSlider, &QSlider::valueChanged, m_subsampleSpinBox, &QSpinBox::setValue);
        connect(m_subsampleSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), m_subsampleSlider, &QSlider::setValue);
        connect(m_subsampleSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsDialog::onSubsampleChanged);
        
        connect(m_refreshRateSlider, &QSlider::valueChanged, m_refreshRateSpinBox, &QSpinBox::setValue);
        connect(m_refreshRateSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), m_refreshRateSlider, &QSlider::setValue);
        connect(m_refreshRateSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsDialog::onRefreshRateChanged);
        
        connect(m_interpolationCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                this, &SettingsDialog::onInterpolationChanged);
    }

    void SettingsDialog::loadSettings()
    {
        // Display resolution
        auto res = m_core->displayResolution();
        m_displayResLabel->setText(QString("%1 x %2").arg(res.x).arg(res.y));
        
        // Subsample candidates
        auto candidates = m_core->subsampleResolutionCandidates();
        if (!candidates.empty()) {
            m_subsampleSlider->setRange(candidates.front().x, candidates.back().x);
            m_subsampleSpinBox->setRange(candidates.front().x, candidates.back().x);
        }
        m_subsampleSpinBox->setValue(m_core->subsampleWidth());
        
        // Refresh rate
        unsigned maxRefresh = m_core->maxRefreshRate();
        m_refreshRateSlider->setRange(1, maxRefresh);
        m_refreshRateSpinBox->setRange(1, maxRefresh);
        m_refreshRateSpinBox->setValue(m_core->refreshRate());
        
        // Interpolation
        m_interpolationCombo->clear();
        const auto& interpolations = m_core->availableInterpolations();
        for (const auto& [name, type] : interpolations) {
            m_interpolationCombo->addItem(QString::fromStdString(name), static_cast<int>(type));
        }
        int currentIdx = m_interpolationCombo->findData(static_cast<int>(m_core->interpolation()));
        if (currentIdx >= 0) {
            m_interpolationCombo->setCurrentIndex(currentIdx);
        }
    }

    void SettingsDialog::onSubsampleChanged(int value)
    {
        m_core->setSubsampleWidth(value);
    }

    void SettingsDialog::onRefreshRateChanged(int value)
    {
        m_core->setRefreshRate(value);
    }

    void SettingsDialog::onInterpolationChanged(int index)
    {
        if (index < 0) return;
        int type = m_interpolationCombo->currentData().toInt();
        m_core->setInterpolation(type);
    }
}
