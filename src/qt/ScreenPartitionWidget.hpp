#pragma once

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QMap>
#include <QColor>

namespace Huenicorn
{
    class HuenicornCore;

    namespace Qt
    {
        /**
         * @brief Widget for visualizing and editing channel screen regions
         */
        class ScreenPartitionWidget : public QWidget
        {
            Q_OBJECT

        public:
            explicit ScreenPartitionWidget(HuenicornCore* core, QWidget* parent = nullptr);

            void refresh();

        signals:
            void channelUVChanged(int channelId);

        protected:
            void paintEvent(QPaintEvent* event) override;
            void mousePressEvent(QMouseEvent* event) override;
            void mouseMoveEvent(QMouseEvent* event) override;
            void mouseReleaseEvent(QMouseEvent* event) override;
            void resizeEvent(QResizeEvent* event) override;

        private:
            struct ChannelRect {
                int channelId;
                QRectF rect;
                QColor color;
                QString name;
                bool active;
            };

            QRectF uvToWidget(float x1, float y1, float x2, float y2) const;
            QPointF widgetToUV(const QPoint& pos) const;
            int findChannelAt(const QPoint& pos) const;
            int findResizeHandle(const QPoint& pos, int channelId) const;

            void updateChannelRects();
            QColor colorForChannel(int index) const;

            HuenicornCore* m_core;
            QVector<ChannelRect> m_channelRects;
            
            int m_selectedChannel{-1};
            int m_resizeHandle{-1}; // 0=topLeft, 1=topRight, 2=bottomLeft, 3=bottomRight
            bool m_isDragging{false};
            QPoint m_dragStart;
            QRectF m_originalRect;

            static constexpr int HandleSize = 10;
        };
    }
}
