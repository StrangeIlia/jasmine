#include "DeloneGeometryFactory.h"

namespace bstu {
/// Оставь надежду, всяк сюда входящий
QGeometryRenderer* DeloneGeometryFactory::create(Polyhedron* polyhedron) {
    QGeometry *geometry = new Qt3DRender::QGeometry();
    SimpleGeometryFactory::createVertexAttribute(geometry, polyhedron);
    DeloneGeometryFactory::createIndexesAttribute(geometry, polyhedron);
    QGeometryRenderer *renderer = new QGeometryRenderer();
    renderer->setGeometry(geometry);
    //renderer->setPrimitiveType(QGeometryRenderer::Triangles);
    renderer->setPrimitiveType(QGeometryRenderer::Lines);
    return renderer;
}

struct Vector2D { float x, y; int position; };
struct Triangle { int first, second, third; };

// a1 = y3 - y1
// a2 = y3 - y2
// b1 = x3 - x1
// b2 = x3 - x2

// a1 * y0 + b1 * x0 = ( a1 * (y3 + y1) + b1 * (x3 + x1) ) / 2
// a2 * y0 + b2 * x0 = ( a2 * (y3 + y2) + b2 * (x3 + x2) ) / 2

// Кол. операций для вычисления коэф.
// 10 сложений/вычитаний
// 5 умножений/делений

// при заморочках с float/double заменив /2 можно 3 умножений и +2 вычитания
// для деления на 2 для float *((unsigned*)float) -= 0x800000

// Для нахождения x0 и y0 нужно
// 3 умножения/деления + 2 сложения/вычитания (+3 вычитания на условную развилку и два abs)
// + деление
// + умножение + вычитание + деление
// значит 6 умножения/деления + 6 сложения/вычитания

// итого для нахождения
// 16 сложений/вычитаний
// 11 умножения/деления

// чтобы найти еще r^2 нужно
// + 3 сложений/вычитаний
// + 2 умножения

// всего
// 19 сложений/вычитаний
// 13 умножения/деления



void DeloneGeometryFactory::createIndexesAttribute(QGeometry* geometry, Polyhedron* polyhedron) {
    throw NotImplementedException();

    int shift = 0;
    int countPoints = 0;
    /// Буфер, хранящий номера точек
    QByteArray bufferIndexes;
    for(int i = 0; i < polyhedron->size(); ++i)
    {
        /// --------------------------------------------------------------------------
        Polygon* polygon = polyhedron->polygon(i);
        int countPointsForPolygon = (polygon->size() - 2) * 3;
        bufferIndexes.reserve(countPoints + countPointsForPolygon);
        if(polygon->size() == 3) {
            bufferIndexes.append((ushort)shift);
            bufferIndexes.append((ushort)shift + 1);
            bufferIndexes.append((ushort)shift + 2);
        } else {


            std::vector<Vector2D> points;

            /// Резервируем место под все точки полигона и суперструктуры
            points.reserve(polygon->size() + 4);

            /// Для определения плоскости, на которую сделаем проекцию
            double x = std::abs(polygon->plane().A);
            double y = std::abs(polygon->plane().B);
            double z = std::abs(polygon->plane().C);

            /// Для определения границы суперструктуры
            float max_x = std::numeric_limits<float>::min();
            float min_x = std::numeric_limits<float>::max();
            float max_y = std::numeric_limits<float>::min();
            float min_y = std::numeric_limits<float>::max();

            /// Границы супер структуры
            points.push_back(Vector2D{0, 0, -1});
            points.push_back(Vector2D{0, 0, -1});
            points.push_back(Vector2D{0, 0, -1});
            points.push_back(Vector2D{0, 0, -1});

            /// Делаем проекцию на одну из плоскостей
            if(x > y) {
                if(x > z) {
                    for(int i = 0; i != polygon->size(); ++i) {
                        Vertex v = polygon->vertex(i);
                        Vector2D vector = Vector2D{v.y, v.z, i};
                        if(vector.x < min_x) min_x = vector.x;
                        if(vector.y < min_x) min_y = vector.y;
                        if(vector.x > max_x) max_x = vector.x;
                        if(vector.y > max_y) max_y = vector.y;
                        points.push_back(vector);

                    }
                } else {
                    for(int i = 0; i != polygon->size(); ++i) {
                        Vertex v = polygon->vertex(i);
                        Vector2D vector = Vector2D{v.x, v.y, i};
                        if(vector.x < min_x) min_x = vector.x;
                        if(vector.y < min_x) min_y = vector.y;
                        if(vector.x > max_x) max_x = vector.x;
                        if(vector.y > max_y) max_y = vector.y;
                        points.push_back(vector);
                    }
                }
            } else if(y > z) {
                for(int i = 0; i != polygon->size(); ++i) {
                    Vertex v = polygon->vertex(i);
                    Vector2D vector = Vector2D{v.x, v.z, i};
                    if(vector.x < min_x) min_x = vector.x;
                    if(vector.y < min_x) min_y = vector.y;
                    if(vector.x > max_x) max_x = vector.x;
                    if(vector.y > max_y) max_y = vector.y;
                    points.push_back(vector);
                }
            } else {
                for(int i = 0; i != polygon->size(); ++i) {
                    Vertex v = polygon->vertex(i);
                    Vector2D vector = Vector2D{v.x, v.y, i};
                    if(vector.x < min_x) min_x = vector.x;
                    if(vector.y < min_x) min_y = vector.y;
                    if(vector.x > max_x) max_x = vector.x;
                    if(vector.y > max_y) max_y = vector.y;
                    points.push_back(vector);
                }
            }
            /// Делаем предварительную сортировку по x
            std::sort(points.begin(), points.end(), [](const Vector2D& v1, const Vector2D& v2) -> bool {
                return v1.x < v2.x;
            });

            auto iter = points.begin();
            while(iter != points.end()) {

            };
        }
        shift += polygon->size();
        countPoints += countPointsForPolygon;
        /// --------------------------------------------------------------------------
    }


    /// Создаем буфер, связанный с геометрией объекта
    Qt3DRender::QBuffer *indexesBuf = new Qt3DRender::QBuffer(geometry);
    /// Передаем в него точки
    indexesBuf->setData(bufferIndexes);
    /// Создаем атрибут отображения геометрии (номера точек)
    QAttribute *indexAttribute = new QAttribute(geometry);
    /// Указываем, какой класс является базовым для номеров
    indexAttribute->setVertexBaseType(QAttribute::UnsignedShort);
    /// Указываем, что атрибут отвечает за номера точек
    indexAttribute->setAttributeType(QAttribute::IndexAttribute);
    /// Передаем точки
    indexAttribute->setBuffer(indexesBuf);
    /// Указываем число точек
    indexAttribute->setCount(bufferIndexes.size() / sizeof(unsigned short));
    /// Добавляем в геометрию объекта наш атрибут
    geometry->addAttribute(indexAttribute);

}

}
