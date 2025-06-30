#include "callout.h"
#include <QChart>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QtMath>
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>

Callout::Callout(QChart *parent)
    : QGraphicsItem(parent),
    m_chart(parent)
{
    m_font.setFamily("Arial");
    m_font.setPointSize(10);

    // 默认隐藏
    setVisible(false);
    setZValue(Z_VALUE); // 确保显示在最上层
}

void Callout::setText(const QString &text) {
    m_text = text;
    updateGeometry();
}

void Callout::setAnchor(const QPointF &point) {
    m_anchor = point;
    updateGeometry();
}

void Callout::updateGeometry() {
    prepareGeometryChange();

    // 计算文本尺寸
    QFontMetrics metrics(m_font);
    m_textRect = metrics.boundingRect(QRect(0, 0, 150, 50),Qt::AlignLeft | Qt::AlignTop, m_text);
    m_textRect.adjust(-5, -5, 5, 5); // 增加边距

    // 计算整体边界
    m_rect = m_textRect;

    // 获取图表视图（安全转换）
    QGraphicsScene *scene = m_chart->scene();
    if (!scene || scene->views().isEmpty()) return;

    QGraphicsView *view = m_chart->scene()->views().first();
    QPointF chartPos = m_chart->mapToPosition(m_anchor);
    QPointF scenePos = m_chart->mapToScene(chartPos.toPoint());
    QPointF viewPos = view->mapFromScene(scenePos);

    // 调整位置
    qreal x = viewPos.x();
    if (x + m_rect.width() > view->width()) {
        x = view->width() - m_rect.width() - 10;
    }

    qreal y = viewPos.y() - m_rect.height() - 15;
    if (y < 10) {
        y = viewPos.y() + 15;
    }

    setPos(view->mapToScene(QPoint(x, y)));
}

QRectF Callout::boundingRect() const {
    return m_rect;
}

void Callout::paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    // 绘制圆角矩形背景
    painter->setBrush(QColor(255, 255, 220, 220));
    painter->setPen(QPen(Qt::black, 1));
    painter->drawRoundedRect(m_rect, 5, 5);

    // 绘制文本
    painter->setFont(m_font);
    painter->setPen(Qt::black);
    painter->drawText(m_textRect, Qt::AlignCenter, m_text);

    // 绘制指向锚点的线
    QPointF anchor = mapFromParent(m_chart->mapToPosition(m_anchor));
    QPointF bottomCenter(m_rect.center().x(), m_rect.bottom());
    painter->drawLine(bottomCenter, anchor);
}
