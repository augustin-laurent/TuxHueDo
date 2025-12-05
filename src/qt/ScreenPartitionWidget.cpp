#include "ScreenPartitionWidget.hpp"
#include <Huenicorn/HuenicornCore.hpp>
#include <Huenicorn/Channel.hpp>

namespace Huenicorn::Qt
{
    ScreenPartitionWidget::ScreenPartitionWidget(HuenicornCore* core, QWidget* parent)
        : QWidget(parent)
        , m_core(core)
    {
        setMinimumSize(400, 300);
        setMouseTracking(true);
        setStyleSheet("background-color: #1a1a2e;");
    }

    void ScreenPartitionWidget::refresh()
    {
        updateChannelRects();
        update();
    }

    void ScreenPartitionWidget::updateChannelRects()
    {
        m_channelRects.clear();
        
        if (!m_core->isInitialized()) return;
        
        const auto& channels = m_core->channels();
        int colorIndex = 0;
        
        for (const auto& [channelId, channel] : channels) {
            ChannelRect rect;
            rect.channelId = channelId;
            rect.active = (channel.state() == Channel::State::Active);
            rect.color = colorForChannel(channelId); // Use channelId for consistent color
            
            const auto& uvs = channel.uvs();
            rect.rect = uvToWidget(uvs.min.x, uvs.min.y, uvs.max.x, uvs.max.y);
            
            // Build device names
            QStringList deviceNames;
            for (const auto& device : channel.devices()) {
                deviceNames << QString::fromStdString(device.name);
            }
            rect.name = QString("Channel %1: [%2]").arg(channelId).arg(deviceNames.join(", "));
            
            m_channelRects.append(rect);
            colorIndex++;
        }
    }

    QRectF ScreenPartitionWidget::uvToWidget(float x1, float y1, float x2, float y2) const
    {
        return QRectF(
            x1 * width(),
            y1 * height(),
            (x2 - x1) * width(),
            (y2 - y1) * height()
        );
    }

    QPointF ScreenPartitionWidget::widgetToUV(const QPoint& pos) const
    {
        return QPointF(
            static_cast<float>(pos.x()) / width(),
            static_cast<float>(pos.y()) / height()
        );
    }

    QColor ScreenPartitionWidget::colorForChannel(int index) const
    {
        static const QVector<QColor> colors = {
            QColor(76, 175, 80, 180),   // Green
            QColor(33, 150, 243, 180),  // Blue
            QColor(255, 152, 0, 180),   // Orange
            QColor(156, 39, 176, 180),  // Purple
            QColor(244, 67, 54, 180),   // Red
            QColor(0, 188, 212, 180),   // Cyan
        };
        return colors[index % colors.size()];
    }

    int ScreenPartitionWidget::findChannelAt(const QPoint& pos) const
    {
        for (int i = m_channelRects.size() - 1; i >= 0; --i) {
            if (m_channelRects[i].active && m_channelRects[i].rect.contains(pos)) {
                return i;
            }
        }
        return -1;
    }

    int ScreenPartitionWidget::findResizeHandle(const QPoint& pos, int channelIdx) const
    {
        if (channelIdx < 0 || channelIdx >= m_channelRects.size()) return -1;
        
        const auto& rect = m_channelRects[channelIdx].rect;
        
        QRectF handles[4] = {
            QRectF(rect.left() - HandleSize/2, rect.top() - HandleSize/2, HandleSize, HandleSize),
            QRectF(rect.right() - HandleSize/2, rect.top() - HandleSize/2, HandleSize, HandleSize),
            QRectF(rect.left() - HandleSize/2, rect.bottom() - HandleSize/2, HandleSize, HandleSize),
            QRectF(rect.right() - HandleSize/2, rect.bottom() - HandleSize/2, HandleSize, HandleSize),
        };
        
        for (int i = 0; i < 4; ++i) {
            if (handles[i].contains(pos)) return i;
        }
        return -1;
    }

    void ScreenPartitionWidget::paintEvent(QPaintEvent* /*event*/)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Draw background
        painter.fillRect(rect(), QColor(26, 26, 46));
        
        // Draw grid
        painter.setPen(QPen(QColor(60, 60, 80), 1, ::Qt::DotLine));
        for (int i = 1; i < 4; ++i) {
            int x = width() * i / 4;
            int y = height() * i / 4;
            painter.drawLine(x, 0, x, height());
            painter.drawLine(0, y, width(), y);
        }
        
        // Draw channel rectangles - iterate in order to ensure all active channels are drawn
        for (int idx = 0; idx < m_channelRects.size(); ++idx) {
            const auto& channelRect = m_channelRects[idx];
            if (!channelRect.active) continue;
            
            // Ensure we have a valid color with good opacity
            QColor fillColor = channelRect.color;
            if (fillColor.alpha() < 100) {
                fillColor.setAlpha(180);
            }
            
            // Draw filled rectangle
            painter.save();
            painter.fillRect(channelRect.rect, fillColor);
            
            // Draw border
            QColor borderColor = fillColor.lighter(130);
            borderColor.setAlpha(255);
            painter.setPen(QPen(borderColor, 2));
            painter.drawRect(channelRect.rect);
            
            // Draw handles at corners
            painter.setBrush(::Qt::white);
            painter.setPen(QPen(::Qt::black, 1));
            QRectF r = channelRect.rect;
            painter.drawEllipse(QPointF(r.left(), r.top()), HandleSize/2, HandleSize/2);
            painter.drawEllipse(QPointF(r.right(), r.top()), HandleSize/2, HandleSize/2);
            painter.drawEllipse(QPointF(r.left(), r.bottom()), HandleSize/2, HandleSize/2);
            painter.drawEllipse(QPointF(r.right(), r.bottom()), HandleSize/2, HandleSize/2);
            
            // Draw label with shadow for better visibility
            painter.setPen(QPen(QColor(0, 0, 0, 150)));
            QFont font = painter.font();
            font.setPointSize(10);
            font.setBold(true);
            painter.setFont(font);
            painter.drawText(channelRect.rect.adjusted(6, 6, -4, -4), 
                           ::Qt::AlignTop | ::Qt::AlignLeft | ::Qt::TextWordWrap,
                           channelRect.name);
            painter.setPen(::Qt::white);
            painter.drawText(channelRect.rect.adjusted(5, 5, -5, -5), 
                           ::Qt::AlignTop | ::Qt::AlignLeft | ::Qt::TextWordWrap,
                           channelRect.name);
            
            painter.restore();
        }
    }

    void ScreenPartitionWidget::mousePressEvent(QMouseEvent* event)
    {
        if (event->button() != ::Qt::LeftButton) return;
        
        int channelIdx = findChannelAt(event->pos());
        if (channelIdx >= 0) {
            m_selectedChannel = channelIdx;
            m_resizeHandle = findResizeHandle(event->pos(), channelIdx);
            m_isDragging = true;
            m_dragStart = event->pos();
            m_originalRect = m_channelRects[channelIdx].rect;
        }
    }

    void ScreenPartitionWidget::mouseMoveEvent(QMouseEvent* event)
    {
        if (!m_isDragging || m_selectedChannel < 0) {
            // Update cursor
            int channelIdx = findChannelAt(event->pos());
            if (channelIdx >= 0) {
                int handle = findResizeHandle(event->pos(), channelIdx);
                if (handle == 0 || handle == 3) {
                    setCursor(::Qt::SizeFDiagCursor);
                } else if (handle == 1 || handle == 2) {
                    setCursor(::Qt::SizeBDiagCursor);
                } else {
                    setCursor(::Qt::SizeAllCursor);
                }
            } else {
                setCursor(::Qt::ArrowCursor);
            }
            return;
        }
        
        QPoint delta = event->pos() - m_dragStart;
        QRectF newRect = m_originalRect;
        
        if (m_resizeHandle >= 0) {
            // Resize
            switch (m_resizeHandle) {
                case 0: // Top-left
                    newRect.setTopLeft(m_originalRect.topLeft() + QPointF(delta));
                    break;
                case 1: // Top-right
                    newRect.setTopRight(m_originalRect.topRight() + QPointF(delta));
                    break;
                case 2: // Bottom-left
                    newRect.setBottomLeft(m_originalRect.bottomLeft() + QPointF(delta));
                    break;
                case 3: // Bottom-right
                    newRect.setBottomRight(m_originalRect.bottomRight() + QPointF(delta));
                    break;
            }
        } else {
            // Move entire rect
            newRect.translate(delta);
        }
        
        // Clamp to widget bounds
        newRect = newRect.normalized();
        if (newRect.left() < 0) newRect.moveLeft(0);
        if (newRect.top() < 0) newRect.moveTop(0);
        if (newRect.right() > width()) newRect.moveRight(width());
        if (newRect.bottom() > height()) newRect.moveBottom(height());
        
        m_channelRects[m_selectedChannel].rect = newRect;
        update();
    }

    void ScreenPartitionWidget::mouseReleaseEvent(QMouseEvent* event)
    {
        if (event->button() != ::Qt::LeftButton || !m_isDragging) return;
        
        if (m_selectedChannel >= 0) {
            // Apply UV changes to core
            const auto& rect = m_channelRects[m_selectedChannel];
            int channelId = rect.channelId;
            
            QPointF uvMin = widgetToUV(rect.rect.topLeft().toPoint());
            QPointF uvMax = widgetToUV(rect.rect.bottomRight().toPoint());
            
            m_core->setChannelUV(channelId, {static_cast<float>(uvMin.x()), static_cast<float>(uvMin.y())}, UVCorner::TopLeft);
            m_core->setChannelUV(channelId, {static_cast<float>(uvMax.x()), static_cast<float>(uvMax.y())}, UVCorner::BottomRight);
            
            emit channelUVChanged(channelId);
        }
        
        m_isDragging = false;
        m_selectedChannel = -1;
        m_resizeHandle = -1;
    }

    void ScreenPartitionWidget::resizeEvent(QResizeEvent* /*event*/)
    {
        updateChannelRects();
    }
}
