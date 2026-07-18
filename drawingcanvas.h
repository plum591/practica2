#ifndef DRAWINGCANVAS_H
#define DRAWINGCANVAS_H

#include <QWidget>
#include <QImage>
#include <QColor>
#include <QPoint>

class DrawingCanvas : public QWidget {
    Q_OBJECT
public:
    explicit DrawingCanvas(QWidget* parent = nullptr);

    void setPenColor(const QColor& color);  // выбрать цвет кисти
    void setPenWidth(int width);            // выбрать толщину кисти
    void setEraser(bool on);                // включить/выключить ластик
    void clear();                           // очистить холст

    void setDrawingEnabled(bool on);        // можно ли рисовать мышкой
    void drawRemoteLine(int x1, int y1, int x2, int y2,
                        const QColor& color, int width);  // линия из сети

signals:
    void lineDrawn(int x1, int y1, int x2, int y2,
                   const QColor& color, int width);  // художник провёл линию

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QImage  m_image;       // картинка, на которой рисуем
    QColor  m_penColor;    // текущий цвет карандаша
    int     m_penWidth;    // текущая толщина
    bool    m_eraser;      // включён ли ластик
    bool    m_drawing;     // нажата ли кнопка мыши
    bool    m_enabled = true;   // разрешено ли рисовать мышкой
    QPoint  m_lastPoint;   // предыдущая точка (чтобы вести линию)
};

#endif
