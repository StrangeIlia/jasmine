#include "ModelViewerWidget.h"
#include <gl/glu.h>

namespace bstu {

ModelViewerWidget::ModelViewerWidget(QWidget *parent) : QGLWidget(parent)
{
    setWindowTitle("Трёхмерная модель");
}


void ModelViewerWidget::setTree(Tree* _tree)
{
    tree = _tree;
}


void ModelViewerWidget::initializeGL()
{
    // Включение сортировки по глубине
    glEnable(GL_DEPTH_TEST);

    // Задаём такой режим смешивания цветов, чтобы работала прозрачность
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Инициализация освещения
    initLighting();
}



void ModelViewerWidget::initLighting()
{
    // Включение освещения
    glEnable(GL_LIGHTING);

    // Включение автоматической нормализации векторов нормалей (обязательно при использовании операций масштабирования)
    glEnable(GL_NORMALIZE);

    // Освещение обеих сторон полигонов
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    // Добавим один источник рассеянного освещения
    GLfloat light_ambient[] = { 0.4, 0.9, 0.9, 1.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glEnable(GL_LIGHT0);

    GLfloat front_color[] = {0.3, 1, 0.3, 1};       // Цвет лицевых граней
    GLfloat back_color[] =  {0.3, 0.3, 1, 1};       // Цвет обратных граней
    glMaterialfv(GL_FRONT, GL_DIFFUSE, front_color);
    glMaterialfv(GL_BACK, GL_DIFFUSE, back_color);
}



void ModelViewerWidget::resizeGL(int nWidth, int nHeight)
{
    // Задание области вывода
    glViewport(0, 0, nWidth, nHeight);

    // Обновляем матрицу центрального проектирования
    resetProjectionMatrix();

    // Обновляем видовую матрицу
    resetModelViewMatrix();
}



void ModelViewerWidget::paintGL()
{
    // Очистка буфера глубины и буфера цвета
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Рисование многогранников
    for (int i=0; i < tree->polyhedronsCount(); i++)
    {
        // Рисование многогранника
        // Если многогранник выделен мышью
        if (selected_polyhedrons.contains(tree->polyhedron(i)))
        {
            // Рисуем одним цветом
            GLfloat selected_color[] = {50/255.0f, 130/255.0f, 200/255.0f, 1};
            glMaterialfv(GL_FRONT, GL_DIFFUSE, selected_color);
        }
        else // Если многогранник не выделен мышью, то рисуем его цветом по умолчанию
        {
            GLfloat front_color[] = {0.3, 1, 0.3, 1};
            glMaterialfv(GL_FRONT, GL_DIFFUSE, front_color);
        }

        // Рисование многогранника
        draw(tree->polyhedron(i));
    }

    /*
    // Рисование сферы
    GLUquadric* sphereObj = gluNewQuadric();
    gluSphere(sphereObj, 1, 100, 100);
    gluDeleteQuadric(sphereObj);
    */

    // Вывод на экран текста
    glColor3f(1, 1, 1);
    QFont text_font("Cambria", 10, QFont::Bold);
    renderText(10, 20, "Колесо мыши - удалить/приблизить", text_font);
    renderText(10, 35, "Кнопка мыши - выделить", text_font);
    renderText(10, 50, "Зажатая кнопка мыши - повернуть сцену", text_font);
    QStringList words = output_text.split('\n');
    for (int i=0; i < words.length(); i++)
     renderText(10, 65 + i*15, words[i], text_font);
}



void ModelViewerWidget::draw(const Polyhedron* polyhedron)
{
    Vertex V;
    Polygon* P;
    // Проходим по всем граням многогранника
    for (int i=0; i < polyhedron->size(); i++)
    {
        P = polyhedron->polygon(i);
        Plane p = P->plane();
        glBegin(GL_POLYGON);
        // Задаём вектор нормали к грани
        glNormal3f(p.A, p.B, p.C);
        // Если вершины многоугольника упорядочены по часовой стрелке
        if (P->isClockwise())
        {
          // Перечисляем координаты вершин в прямом порядке
          for (int j=0; j < P->size();j++)
          {
            V = P->vertex(j);
            glVertex3fv((float*)&V);
          }
        }
        else // Если вершины многоугольника упорядочены против часовой стрелки
        {
          // Перечисляем координаты вершин в обратном порядке
          for (int j=P->size()-1; j >= 0;j--)
          {
            V = P->vertex(j);
            glVertex3fv((float*)&V);
          }
        }
        glEnd();
    }
}



void ModelViewerWidget::resetProjectionMatrix()
{
    // Переключение на матрицу проектирования
    glMatrixMode(GL_PROJECTION);
    // Загрузка единичной матрицы проектирования
    glLoadIdentity();
    // Инициализация перспективной проекции
    gluPerspective(30.0, (float)width() / height(), 0.1, 20);
}


void ModelViewerWidget::resetModelViewMatrix()
{
    // Переключение на видовую матрицу
    glMatrixMode(GL_MODELVIEW);
    // Инициализация видовой матрицы в виде единичной матрицы
    glLoadIdentity();
    // Перенос вглубь экрана
    glTranslatef(0, 0, -zoffset);
    // Поворот объекта
    glMultMatrixf(rotateMatrix.transposed().data());
    // Масштабирование
    glScalef(0.3, 0.3, 0.3);

    // Получим текущие матрицы OpenGL для проектирования и аффинных преобразований
    QMatrix4x4 projectionMatrix, modelViewMatrix;
    glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix.data());
    glGetFloatv(GL_MODELVIEW_MATRIX,  modelViewMatrix.data());

    // Умножим матрицы проектирования и видовую матрицы
    QMatrix4x4 Q = projectionMatrix * modelViewMatrix;

    // Вычислим обратную матрицу
    IQ = Q.inverted();
}



// Обработчик события перемещения указателя мыши (событие происходит при зажатой кнопке мыши)
void ModelViewerWidget::mouseMoveEvent(QMouseEvent* m_event)
{
    // Вычислим, на сколько переместился курсор мыши между двумя событиями mouseMoveEvent
    QPoint dp = m_event->pos() - mousePosition;
    // Изменим матрицу поворота в соответствии с тем, как пользователь переместил курсор мыши
    rotateMatrix.rotate(-dp.x(), 0, 1, 0);         // Умножение R на матрицу поворота вокруг оси y
    rotateMatrix.rotate(-dp.y(), 1, 0, 0);         // Умножение R на матрицу поворота вокруг оси x
    // Сохраним текущую позицию мыши
    mousePosition = m_event->pos();
    // Обновим видовую матрицу
    resetModelViewMatrix();
    updateGL(); // Перерисовать окно
}



// Обработчик события прокрутки колеса мыши
void ModelViewerWidget::wheelEvent(QWheelEvent* w_event)
{
    // При прокрутке колеса мыши изменяем глубину объекта
    zoffset -= w_event->delta() / 500.0;
    resetModelViewMatrix(); // Обновим видовую матрицу
    updateGL(); // Перерисовать окно
}


// Возвращает глубину точки A
float ModelViewerWidget::depth(const QVector3D &A) const
{
    float M[16];
    glGetFloatv(GL_MODELVIEW_MATRIX,  M);
    // Для определения глубины точки A нужно умножить третью строку видовой матрицы на однородные координаты точки A
    return -(M[2]*A.x() + M[6]*A.y() + M[10]*A.z() + M[14]);
}



QPointF ModelViewerWidget::toOpenGLScreen(QPoint pos) const
{
    float mx = -1.0f + 2.0f * (double) pos.x() / width();
    float my =  1.0f - 2.0f * (double) pos.y() / height();
    return QPointF(mx, my);
}



Ray ModelViewerWidget::selectionRay(const QPoint& P) const
{
    // Вычислим координаты указателя мыши в экранной системе координат OpenGL
    QPointF M = toOpenGLScreen(P);
    // Вычислим параметры селектирующего луча
    // Для этого нужно взять две точки, прямая через которые перпендикулярна экранной плоскости и пересекает её в точке P(x, y)
    // Первая точка A должна находится на ближней плоскости отсечения (z = -1), вторая точка B - на дальней плоскости отсечения (z = 1)
    QVector3D A = (IQ * QVector4D(M.x(), M.y(), -1, 1)).toVector3DAffine();
    QVector3D B = (IQ * QVector4D(M.x(), M.y(),  1, 1)).toVector3DAffine();
    return Ray(Vertex(A.x(), A.y(), A.z()), Vertex(B.x(), B.y(), B.z()));
}



void ModelViewerWidget::mousePressEvent(QMouseEvent* m_event)
{
    mousePosition = m_event->pos();

    // Рассчитаем параметры селектирующего луча
    Ray selection_Ray = selectionRay(mousePosition);

    // Проверим, какие объекты лежат на пути селектирующего луча и вычислим их глубину (depth)
    output_text = "";

    Polygon* P;
    Vertex R; // Координаты точки пересечения луча с многоугольником
    Polyhedron* polyhedron;

    selected_polyhedrons.clear(); // Очистим массив пересечённых мышью многогранников
    // Проходим по всем многогранникам
    for (int i=0; i < tree->polyhedronsCount(); i++)
    {
        polyhedron = tree->polyhedron(i);
        // Проходим по всем граням многогранника
        for (int j=0; j < polyhedron->size(); j++)
        {
            P = polyhedron->polygon(j);
            // Если селектирующий луч пересекает многоугольник
            if (P->intersects(selection_Ray, R))
            {
                // Добавить многогранник в массив пересечённых лучом многогранников
                selected_polyhedrons.push_back(polyhedron);
                break;
            }
        }
    }

   /* QVector3D R[2]; // Координаты точек пересечения
    for (int i=0; i < 7;i++)
    {
        if (cubes[i].is_selecting = cubes[i].intersects(selection_Ray, R) > 0)
        {
            output_text = output_text + QString("Куб %1. Глубины: Z1=%2, Z2=%3.\n").arg(i+1).arg(depth(R[0])).arg(depth(R[1]));
        }
    }
    for (int i=0; i < 4; i++)
    {
       if(spheres[i].is_selecting = spheres[i].intersects(selection_Ray, R) > 0)
       {
           output_text = output_text + QString("Сфера %1. Глубины: Z1=%2, Z2=%3.\n").arg(i+1).arg(depth(R[0])).arg(depth(R[1]));
       }
    }*/
    updateGL(); // Перерисовать окно
}


void ModelViewerWidget::keyPressEvent(QKeyEvent* event)
{
    // Закрыть окно при нажатии клавиши Escape
    if (event->key() == Qt::Key_Escape)
    {
        close();
    }
}



} // namespace bstu
