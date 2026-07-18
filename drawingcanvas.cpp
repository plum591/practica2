#include "drawingcanvas.h"
#include <QPainter>
#include <QMouseEvent>

DrawingCanvas::DrawingCanvas(QWidget* parent)
    : QWidget(parent),
      m_penColor(Qt::black),
      m_penWidth(3),
      m_eraser(false),
      m_drawing(false)
{
    m_image = QImage(size(), QImage::Format_RGB32);
    m_image.fill(Qt::white);
}

void DrawingCanvas::setPenColor(const QColor& color) {
    m_penColor = color;
    m_eraser = false;
}

void DrawingCanvas::setPenWidth(int width) {
    m_penWidth = width;
}

void DrawingCanvas::setEraser(bool on) {
    m_eraser = on;
}

void DrawingCanvas::clear() {
    m_image.fill(Qt::white);
    update();
}

void DrawingCanvas::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.drawImage(0, 0, m_image);
}

void DrawingCanvas::mousePressEvent(QMouseEvent* event) {
    if (!m_enabled) return;
    if (event->button() == Qt::LeftButton) {
        m_drawing = true;
        m_lastPoint = event->pos();
    }
}

void DrawingCanvas::mouseMoveEvent(QMouseEvent* event) {
    if (!m_enabled) return;
    if (m_drawing) {
        QColor color = m_eraser ? Qt::white : m_penColor;
        int width = m_eraser ? (m_penWidth + 8) : m_penWidth;

        QPoint from = m_lastPoint;
        QPoint to   = event->pos();

        QPainter painter(&m_image);
        QPen pen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        painter.drawLine(from, to);
        update();

        emit lineDrawn(from.x(), from.y(), to.x(), to.y(), color, width);

        m_lastPoint = to;
    }
}

void DrawingCanvas::resizeEvent(QResizeEvent*) {
    if (width() > m_image.width() || height() > m_image.height()) {
        QImage newImage(size(), QImage::Format_RGB32);
        newImage.fill(Qt::white);
        QPainter painter(&newImage);
        painter.drawImage(0, 0, m_image);
        m_image = newImage;
    }
}

void DrawingCanvas::setDrawingEnabled(bool on) {
    m_enabled = on;
    m_drawing = false;
}

void DrawingCanvas::drawRemoteLine(int x1, int y1, int x2, int y2, const QColor& color, int width) {
    QPainter painter(&m_image);
    QPen pen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    painter.drawLine(QPoint(x1, y1), QPoint(x2, y2));
    update();
}