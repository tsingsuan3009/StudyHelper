#ifndef CALLOUT_H
#define CALLOUT_H

#include <QGraphicsItem>
#include <QFont>

class QChart;

class Callout : public QGraphicsItem
{
public:
    explicit Callout(QChart *parent = nullptr);

    void setText(const QString &text);
    void setAnchor(const QPointF &point);
    void updateGeometry();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    QString m_text;
    QRectF m_textRect;
    QRectF m_rect;
    QPointF m_anchor;
    QFont m_font;
    QChart *m_chart;

    // 样式常量
    static const int PADDING = 5;
    static const int CORNER_RADIUS = 5;
    static const int Z_VALUE = 11;

};

#endif // CALLOUT_H
