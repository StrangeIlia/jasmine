#ifndef MODELVIEWERWIDGET_H
#define MODELVIEWERWIDGET_H

#include <QGLWidget>
#include <QMatrix4x4>
#include <QMouseEvent>
#include "geometry/Tree.h"

namespace bstu {

class ModelViewerWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit ModelViewerWidget(QWidget *parent = 0);

    void setTree(Tree* tree);

private:

    Tree* tree;

    // Массив многогранников, выделенных мышью
    QVector<Polyhedron*> selected_polyhedrons;

    // Глубина объекта
    float zoffset = 3;

    // Матрица поворота, используемая для поворота сцены, когда пользователь зажимает кнопку мыши
    QMatrix4x4 rotateMatrix;

    // Матрица, обратная матрице Q (Q - произведение видовой матрицы и матрицы проектирования)
    QMatrix4x4 IQ;

    // Позиция указателя мыши
    QPoint mousePosition;

    // Текст, который выводится на экране
    QString output_text;

    // Инициализация освещения
    void initLighting();

    // Подпрограмма для рисования куба
    void draw(const Polyhedron* polyhedron);

    // Процедура для изменения матрицы проектирования
    void resetProjectionMatrix();

    // Процедура для изменения видовой матрицы
    void resetModelViewMatrix();

    ///
    /// \brief toOpenGLScreen
    /// \param pos - позиция указателя мыши в оконной системе координат
    /// \return Возвращает координаты указателя мыши в экранной системе координат OpenGL (от -1 до 1), соответствующую точке pos
    ///
    QPointF toOpenGLScreen(QPoint pos) const;

    ///
    /// \brief depth
    /// \param A
    /// \return Возвращает глубину точки (расстояние до экранной плоскости)
    /// Эту функцию можно использовать для определения того, какой из выделенных объектов ближе к наблюдателю,
    /// и, например, сортировать объекты по глубине
    float depth(const QVector3D& A) const;

    ///
    /// \brief selectionRay
    /// \return Возвращает параметры селектирующего луча, выпущенного вглубь экрана из точки P(X, Y)
    /// Для расчёта параметров луча используются текущие матрицы OpenGL
    Ray selectionRay(const QPoint& P) const;

    // Процедура для инициализации параметров OpenGL
    void initializeGL();
    void resizeGL(int nWidth, int nHeight);
    void paintGL();
    void mouseMoveEvent(QMouseEvent* m_event);
    void wheelEvent(QWheelEvent* w_event);
    void keyPressEvent(QKeyEvent* key_event);

    // Обработчик нажатия кнопок мыши
    void mousePressEvent(QMouseEvent* m_event);

signals:

public slots:
};

} // namespace bstu

#endif // MODELVIEWERWIDGET_H
