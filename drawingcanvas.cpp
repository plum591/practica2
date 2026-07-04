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
    // Белый фон холста.
    m_image = QImage(size(), QImage::Format_RGB32);
    m_image.fill(Qt::white);
}

void DrawingCanvas::setPenColor(const QColor& color) {
    m_penColor = color;
    m_eraser = false;   // выбор цвета автоматически выключает ластик
}

void DrawingCanvas::setPenWidth(int width) {
    m_penWidth = width;
}

void DrawingCanvas::setEraser(bool on) {
    m_eraser = on;
}

void DrawingCanvas::clear() {
    m_image.fill(Qt::white);
    update();   // попросить Qt перерисовать виджет
}

void DrawingCanvas::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.drawImage(0, 0, m_image);
}

void DrawingCanvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_drawing = true;
        m_lastPoint = event->pos();
    }
}

void DrawingCanvas::mouseMoveEvent(QMouseEvent* event) {
    if (m_drawing) {
        QPainter painter(&m_image);

        // Ластик = рисуем белым цветом и чуть толще.
        QColor color = m_eraser ? Qt::white : m_penColor;
        int width = m_eraser ? (m_penWidth + 8) : m_penWidth;

        QPen pen(color, width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        painter.drawLine(m_lastPoint, event->pos());

        m_lastPoint = event->pos();
        update();
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
