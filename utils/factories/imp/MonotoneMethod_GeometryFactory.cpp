#include "MonotoneMethod_GeometryFactory.h"

namespace bstu {

QGeometryRenderer* MonotoneMethod_GeometryFactory::create(Polyhedron* polyhedron) {
    QGeometry *geometry = new Qt3DRender::QGeometry();
    SimpleGeometryFactory::createVertexAttribute(geometry, polyhedron);
    MonotoneMethod_GeometryFactory::createIndexesAttribute(geometry, polyhedron);
    QGeometryRenderer *renderer = new QGeometryRenderer();
    renderer->setGeometry(geometry);
    renderer->setPrimitiveType(QGeometryRenderer::Triangles);
    return renderer;
}

struct Vector2D {
    float x, y;
    inline Vector2D operator + (const Vector2D& v) {
        return Vector2D{x + v.x, y + v.y};
    }
    inline Vector2D operator - (const Vector2D& v) {
        return Vector2D{x - v.x, y - v.y};
    }
};

struct Edge { unsigned top; unsigned bottom; };
struct TriangleIndexes { unsigned short first, second, third; };

TriangleIndexes normalize(TriangleIndexes triangle);

struct MonotonePolygon {
    unsigned afterTheEnd, beforeTheBeginning; /// Следующие точки
    QStack<unsigned> stack; /// Стек вершин, которые не были триангулированы
    explicit MonotonePolygon(unsigned point, unsigned next_1, unsigned next_2) {
        afterTheEnd = next_1;
        beforeTheBeginning = next_2;
        stack.push(point);
    }
    /// Возращает истина, если требуется одна точка, чтобы завершить полигон
    inline bool requiresTheLastPoint() const {
        return afterTheEnd == beforeTheBeginning;
    }
    /// Возращает предыдущую точку в полигоне
    inline unsigned giveThePreviousPoint(unsigned point) const {
        if(afterTheEnd == point) {
            return stack.last(); // <=> stack.top()
        } else {
            return stack.first();
        }
    }
};

class Triangulator {
    /// Точки
    std::vector<Vector2D> points;
    /// Индексы точек
    std::vector<unsigned> indexesPoints;
    /// Добавленные ребра
    /// Есть конечно insertMulti, но он не сортирует value
    /// Поэтому, будем сортировать в ручную
    QHash<unsigned, unsigned> addedEdges;
    /// Используется для быстрого поиска полигона
    QHash<unsigned, MonotonePolygon*> polygonsHash;
    /// Преобразует трехмерную точку в двумерную
    typedef std::function<Vector2D(Vertex)> VectorSelector;
    /// Для упрощения
    typedef std::vector<unsigned>::iterator PointIterator;
public:
    typedef std::function<void(TriangleIndexes)> Writer;
    void triangulate(Polygon* polygon, Writer writer);
private:
    static bool isClockWise(Plane plane);
    // Используется для создания проекции
    VectorSelector getSelector(Plane plane) const;
    // Разбиение на монотонные многоугольники
    void fragmentation();
    // Триангуляция монотонных многоугольников
    void triangulate(Writer writer);
    // Вспомогательные функции
    // Удаляет ребра, которые на пересекаются с заметающей прямой
    void clearEdges(QLinkedList<Edge> &edges, unsigned point) const;
    //
    bool isClockWise(unsigned left, unsigned center, unsigned right) const;
    // Ищет боковые вершины для указанной точки из множества ребер
    void findEdges(const QLinkedList<Edge>& edges, unsigned point, Edge& left, Edge& right) const;
    // Поиск точки ближайшей по y точки,
    // лежащей между указанными ребрами и находящаяся выше указанной точки
    unsigned findTopPoint(PointIterator point, Edge left, Edge right);
    // Тоже, что и прошлая функция, но точка ниже
    unsigned findBottomPoint(PointIterator point, Edge left, Edge right);
    // Добавить ребро (используется для разбиения на монотонные полигоны)
    void addEdge(unsigned top, unsigned bottom);
    // Добавляет ребро (используется для заметающей прямой)
    void addOperativeEdge(QLinkedList<Edge> &edges, unsigned top, unsigned bottom) const;
    // Упорядочивает точки относительно center
    bool exchangeRequired(unsigned center, unsigned left, unsigned right);
    // Упорядочивает полигоны относительно center
    bool exchangeRequired(unsigned center, MonotonePolygon* left, MonotonePolygon* right);
    // Добавляет точку в полигон
    // По максимому триангулирует монотонный многоугольник
    // vectors ему нужен, для некоторых мат. операций
    void addPoint(MonotonePolygon* polygon, unsigned point, unsigned nextPoint, Writer writer);
    // Обработка точки, противоположной дуги
    void oppositePointProcessing(MonotonePolygon* polygon, unsigned point, Writer writer);
    // Обработка точки, лежащей на дуге
    void arcProcessing(MonotonePolygon* polygon, unsigned point,  Writer writer);
};

void MonotoneMethod_GeometryFactory::createIndexesAttribute(QGeometry* geometry, Polyhedron* polyhedron) {
    int shift = 0;
    int countPoints = 0;

    /// Буфер, хранящий номера точек
    QByteArray bufferIndexes;
    Triangulator triangulator;
    const auto writer = [&shift, &bufferIndexes] (TriangleIndexes triangle) -> void {
        triangle.first += shift;
        triangle.second += shift;
        triangle.third += shift;
        bufferIndexes.append((char*)&triangle, sizeof(unsigned short) * 3);
    };

    for(int i = 0; i < polyhedron->size(); ++i)
    {
        Polygon* polygon = polyhedron->polygon(i);
        int countPointsForPolygon = (polygon->size() - 2) * 3;
        bufferIndexes.reserve(countPoints + countPointsForPolygon);
        triangulator.triangulate(polygon, writer);
        shift += polygon->size();
        countPoints += countPointsForPolygon;
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

void Triangulator::triangulate(Polygon* polygon, Writer writer) {
    points.clear();
    indexesPoints.clear();
    points.reserve(polygon->size());
    indexesPoints.reserve(polygon->size());
    VectorSelector selector = getSelector(polygon->plane());
    if(isClockWise(polygon->plane())/*polygon->isClockwise()*/) {
        for(unsigned i = 0; i != polygon->size(); ++i) {
            points.push_back(selector(polygon->vertex(i)));
            indexesPoints.push_back(i);
        }
    } else {
        unsigned lastIndex = polygon->size() - 1;
        for(unsigned i = 0; i != polygon->size(); ++i) {
            points.push_back(selector(polygon->vertex(lastIndex - i)));
            indexesPoints.push_back(i);
        }
    }

    auto comparator = [dataPtr = points.data()] (unsigned top, unsigned bottom) -> bool {
        if(dataPtr[top].y == dataPtr[bottom].y) {
            return dataPtr[top].x < dataPtr[bottom].x;
        } else {
            return dataPtr[top].y > dataPtr[bottom].y;
        }
    };
    std::sort(indexesPoints.begin(), indexesPoints.end(), comparator);
    fragmentation();
    triangulate(writer);
}

TriangleIndexes normalize(TriangleIndexes triangle) {
    if(triangle.first > triangle.second) {
        std::swap(triangle.first, triangle.second);
    }
    if(triangle.second > triangle.third) {
        std::swap(triangle.second, triangle.third);
    }
    if(triangle.first > triangle.second) {
        std::swap(triangle.first, triangle.second);
    }
    return triangle;
}

bool Triangulator::isClockWise(Plane plane) {
    double x = std::abs(plane.A);
    double y = std::abs(plane.B);
    double z = std::abs(plane.C);

    if(x > y && x > z)  return plane.A < 0;
    else if(y > z)      return plane.B < 0;
    else                return plane.C < 0;
}

Triangulator::VectorSelector Triangulator::getSelector(Plane plane) const {
    /// Для определения плоскости, на которую сделаем проекцию
    double x = std::abs(plane.A);
    double y = std::abs(plane.B);
    double z = std::abs(plane.C);

    /// Интересный факт
    /// Для сохранения направления обхода нужно инвертировать
    /// одну из осей, если направление нормали не совпадает с нормалью плоскости
    if(x > y && x > z) {
        // (0, 1, 0) x (0, 0, 1) = (1, 0, 0)
        if(plane.A > 0) {
            return [] (Vertex v) -> Vector2D {
                return Vector2D{v.y, v.z};
            };
        } else {
            return [] (Vertex v) -> Vector2D {
                return Vector2D{v.z, v.y};
            };
        }
    } else if(y > z) {
        // (0, 0, 1) x (1, 0, 0) = (0, 1, 0)
        if(plane.B > 0) {
            return [] (Vertex v) -> Vector2D {
                return Vector2D{v.z, v.x};
            };
        } else {
            return [] (Vertex v) -> Vector2D {
                return Vector2D{v.x, v.z};
            };
        }
    } else {
        // (1, 0, 0) x (0, 1, 0) = (0, 0, 1)
        if(plane.C > 0) {
            return [] (Vertex v) -> Vector2D {
                return Vector2D{v.x, v.y};
            };
        } else {
            return [] (Vertex v) -> Vector2D {
                return Vector2D{v.y, v.x};
            };
        }
    }
}

void Triangulator::clearEdges(QLinkedList<Edge> &edges, unsigned point) const {
    float req_y = points[point].y;
    while(!edges.isEmpty() && req_y < points[edges.back().bottom].y)
        edges.removeLast();
}

bool Triangulator::isClockWise(unsigned left, unsigned center, unsigned right) const {
    Vector2D a, b;
    a.x = points[center].x - points[left].x;
    a.y = points[center].y - points[left].y;
    b.x = points[center].x - points[right].x;
    b.y = points[center].y - points[right].y;
    return (a.x * b.y - a.y * b.x) <= 0;
}

void Triangulator::findEdges(const QLinkedList<Edge>& edges, unsigned point, Edge& left, Edge& right) const {
    Vector2D current = points[point];
    float left_x = -std::numeric_limits<float>::max();
    float rigth_x = std::numeric_limits<float>::max();
    for(auto iter = edges.begin(); iter != edges.end(); ++iter) {
        /// Исключительная ситуация для merge вершин
        if(iter->top == point || iter->bottom == point) continue;

        Vector2D top = points[iter->top];
        Vector2D bottom = points[iter->bottom];

        float intersection = top.x + (bottom.x - top.x) / (bottom.y - top.y) * (current.y - top.y);
        if(intersection < current.x && intersection > left_x) {
            left_x = intersection;
            left = *iter;
        }
        if(intersection > current.x && intersection < rigth_x) {
            rigth_x = intersection;
            right = *iter;
        }
    };
}

unsigned Triangulator::findTopPoint(PointIterator iterator, Edge left, Edge right) {
    unsigned minPoint;
    if(points[right.top].y < points[left.top].y)
        minPoint = right.top;
    else
        minPoint = left.top;

    --iterator;
    /// Опять исключительные ситуации
    if(*iterator == right.bottom || *iterator == left.bottom) {
        --iterator;
    }
    if(*iterator == right.bottom || *iterator == left.bottom) {
        --iterator;
    }
    for( ; *iterator != minPoint; --iterator) {
        Vector2D current = points[*iterator];
        Vector2D top = points[left.top];
        Vector2D bottom = points[left.bottom];
        float intersection = top.x + (bottom.x - top.x) / (bottom.y - top.y) * (current.y - top.y);
        /// Точка пересечения с левым ребром должна быть правее самой точки
        if(intersection > current.x) continue;
        top = points[right.top];
        bottom = points[right.bottom];
        intersection = top.x + (top.x - bottom.x) / (top.y - bottom.y) * (top.y - current.y);
        /// И левее правого ребра
        if(intersection < current.x) continue;
        return *iterator;
    }
    return minPoint;
}

unsigned Triangulator::findBottomPoint(PointIterator iterator, Edge left, Edge right)  {
    unsigned minPoint;
    if(points[right.bottom].y < points[left.bottom].y)
        minPoint = left.bottom;
    else
        minPoint = right.bottom;

    ++iterator;
    /// И снова исключительные ситуации
    if(*iterator == right.top || *iterator == left.top) {
        ++iterator;
    }
    if(*iterator == right.top || *iterator == left.top) {
        ++iterator;
    }

    for(; *iterator != minPoint; ++iterator) {
        Vector2D current = points[*iterator];
        Vector2D top = points[left.top];
        Vector2D bottom = points[left.bottom];
        float intersection = top.x + (bottom.x - top.x) / (bottom.y - top.y) * (current.y - top.y);
        /// Точка пересечения с левым ребром должна быть правее самой точки
        if(intersection > current.x) continue;
        top = points[right.top];
        bottom = points[right.bottom];
        intersection = top.x + (top.x - bottom.x) / (top.y - bottom.y) * (top.y - current.y);
        /// И левее правого ребра
        if(intersection < current.x) continue;
        return *iterator;
    }
    return minPoint;
}

void Triangulator::addEdge(unsigned top, unsigned bottom) {
    addedEdges.insertMulti(top, bottom);
}

void Triangulator::addOperativeEdge(QLinkedList<Edge> &edges, unsigned top, unsigned bottom) const {
    float y = points[bottom].y;
    auto iter = edges.begin();
    while(iter != edges.end() && y > points[iter->bottom].y) {
        ++iter;
    }
    edges.insert(iter, Edge{top, bottom});
}

bool Triangulator::exchangeRequired(unsigned center, unsigned left, unsigned right) {
    Vector2D vcenter = points[center];
    Vector2D vleft = points[left] - vcenter;
    Vector2D vright = points[right] - vcenter;

    return vleft.x * std::abs(vright.y) > vright.x * std::abs(vleft.y);
}

bool Triangulator::exchangeRequired(unsigned center, MonotonePolygon* left, MonotonePolygon* right)  {
    unsigned leftPoint, rightPoint;
    if(left->afterTheEnd == center) {
        if(left->stack.size() == 1)
            leftPoint = left->beforeTheBeginning;
        else
            leftPoint = left->stack[left->stack.size() - 2];
    } else {
        if(left->stack.size() == 1)
            leftPoint = left->afterTheEnd;
        else
            leftPoint = left->stack[1];
    }
    if(right->afterTheEnd == center) {
        if(right->stack.size() == 1)
            rightPoint = right->beforeTheBeginning;
        else
            rightPoint = right->stack[right->stack.size() - 2];
    } else {
        if(right->stack.size() == 1)
            rightPoint = right->afterTheEnd;
        else
            rightPoint = right->stack[1];
    }
    return exchangeRequired(center, leftPoint, rightPoint);
}

// Добавляет в полигон новую точку
void Triangulator::addPoint(MonotonePolygon* polygon, unsigned point, unsigned nextPoint, Writer writer) {
    if(polygon->stack.size() < 2) {
        polygon->stack.push(point);
        if(polygon->beforeTheBeginning == point){
            polygon->beforeTheBeginning = polygon->afterTheEnd;
        }
        polygon->afterTheEnd = nextPoint;
    } else {
        if(polygon->beforeTheBeginning == point) {
             oppositePointProcessing(polygon, point, writer);
             polygon->beforeTheBeginning = nextPoint;
        } else {
            arcProcessing(polygon, point, writer);
            polygon->afterTheEnd = nextPoint;
        }
    }
}

void Triangulator::oppositePointProcessing(MonotonePolygon* polygon, unsigned point, Writer writer) {
    unsigned top = polygon->stack.pop(); // Запоминаем точку с вершины стека
    TriangleIndexes triangle;
    triangle.first = point;
    triangle.third = top;
    while(!polygon->stack.empty()) {
        triangle.second = triangle.third;
        triangle.third = polygon->stack.pop();
        /// Здесь нужна нормализация, поскольку неизвестно направление обхода
        TriangleIndexes result = normalize(triangle);
        writer(result);
    }
    polygon->stack.push(point);
    polygon->stack.push(top);
}

void Triangulator::arcProcessing(MonotonePolygon* polygon, unsigned point, Writer writer) {
    TriangleIndexes triangle;
    triangle.first = point;
    triangle.second = polygon->stack.pop(); // Запоминаем точку с вершины стека
    triangle.third = polygon->stack.top(); // берем еще одну точку из стека
    while(true) {
        TriangleIndexes result = normalize(triangle);
        if(!isClockWise(result.first, result.second, result.third))
            break;
        writer(result);
        triangle.second = triangle.third;
        polygon->stack.pop();
        if(polygon->stack.empty()) break;
        triangle.third = polygon->stack.top();
    }
    polygon->stack.push(triangle.second);
    polygon->stack.push(point);
}

void Triangulator::fragmentation() {
    QLinkedList<Edge> usedEdges; /// Используемые ребра
    for(auto i = indexesPoints.begin(); i != indexesPoints.end(); ++i) {
        unsigned point = *i;
        unsigned predPoint = point == 0 ? indexesPoints.size() - 1 : point - 1;
        unsigned nextPoint = point == indexesPoints.size() - 1 ? 0 : point + 1;
        bool predBelow = points[point].y > points[predPoint].y;
        bool nextBelow = points[point].y > points[nextPoint].y;

        clearEdges(usedEdges, point);

        bool specialPoint = nextBelow == predBelow;
        bool splitPoint = nextBelow;
        if(!specialPoint) {
            /// Здесь обработка очень противной исключительной ситуации,
            /// когда у нас есть несколько точек на одном уровне.
            /// Если одна точка ниже, а вторая на том же уровне
            /// То точка может быть особой, если она крайне правая
            if(points[point].y == points[predPoint].y) {
                specialPoint = points[point].x > points[predPoint].x;
            }
            else if(points[point].y == points[nextPoint].y) {
                specialPoint = points[point].x > points[nextPoint].x;
                splitPoint = predBelow;
            }
        }

        if(specialPoint && !isClockWise(predPoint, point, nextPoint)) {
            Edge left, right;
            findEdges(usedEdges, point, left, right);
            if(splitPoint) {
                /// split вершина
                addEdge(findTopPoint(i, left, right), point);
            } else {
                /// merge вершина
                addEdge(point,findBottomPoint(i, left, right));
            }
        }

        if(predBelow) {
            addOperativeEdge(usedEdges, point, predPoint);
        }
        if(nextBelow) {
            addOperativeEdge(usedEdges, point, nextPoint);
        }
    }
}

void Triangulator::triangulate(Writer writer) {
    unsigned point;
    /// Буфер точек, являющиеся окончанием ребер из точки point
    std::vector<unsigned> bufferPoints;
    /// В теории, 3 точки это максимум
    bufferPoints.reserve(3);

    const auto comparator = [&point, this] (unsigned left, unsigned right) {
        return this->exchangeRequired(point, left, right);
    };

    for(auto i = indexesPoints.begin(); i != indexesPoints.end(); ++i) {
        point = *i;
        unsigned leftPoint = point == 0 ? indexesPoints.size() - 1 : point - 1;
        unsigned rightPoint = point == indexesPoints.size() - 1 ? 0 : point + 1;
        auto containerPolygonsIters = polygonsHash.equal_range(point);
        bool findPolygon = containerPolygonsIters.first != containerPolygonsIters.second;
        MonotonePolygon *firstPolygon = nullptr, *secondPolygon = nullptr;
        while(containerPolygonsIters.first != containerPolygonsIters.second) {
            MonotonePolygon *polygon = containerPolygonsIters.first.value();
            if(polygon->requiresTheLastPoint()) {
                /// Поскольку это последняя точка, то следующая ничего не значит
                addPoint(polygon, point, 0, writer);
                delete polygon;
            } else {
                if(firstPolygon == nullptr){
                    firstPolygon = polygon;
                } else {
                    secondPolygon = polygon;
                }
           }
           ++containerPolygonsIters.first;
        }

        polygonsHash.remove(point);

        auto iterPair = addedEdges.equal_range(point);
        if(iterPair.first != addedEdges.end()) {
            /// Если у нас есть доп. ребра
            bufferPoints.clear();
            while(iterPair.first != iterPair.second) {
                bufferPoints.push_back(iterPair.first.value());
                ++iterPair.first;
            }
            std::sort(bufferPoints.begin(), bufferPoints.end(), comparator);
            auto iterator = bufferPoints.begin();
            unsigned edgeLeft = *iterator;
            while(++iterator != bufferPoints.end()) {
                unsigned edgeRight = *iterator;
                MonotonePolygon* polygon = new MonotonePolygon(point, edgeLeft, edgeRight);
                polygonsHash.insertMulti(edgeLeft, polygon);
                polygonsHash.insertMulti(edgeRight, polygon);
                edgeLeft = edgeRight;
            }

            addedEdges.remove(point);

            if(firstPolygon == nullptr) {
                /// Для стартовой точки количество доп. ребер не больше 1
                MonotonePolygon* polygon;
                polygon = new MonotonePolygon(point, leftPoint, bufferPoints.front());
                polygonsHash.insertMulti(leftPoint, polygon);
                polygonsHash.insertMulti(bufferPoints.front(), polygon);
                polygon = new MonotonePolygon(point, bufferPoints.front(), rightPoint);
                polygonsHash.insertMulti(rightPoint, polygon);
                polygonsHash.insertMulti(bufferPoints.front(), polygon);
            }
            else {
                if(secondPolygon == nullptr) {
                    if(bufferPoints.size() > 1 && exchangeRequired(point, leftPoint, rightPoint))
                        std::swap(leftPoint, rightPoint);
                    if(firstPolygon->giveThePreviousPoint(point) == leftPoint) {
                        addPoint(firstPolygon, point, bufferPoints.front(), writer);
                        if(!firstPolygon->requiresTheLastPoint())
                            polygonsHash.insertMulti(bufferPoints.front(), firstPolygon);

                        MonotonePolygon* polygon = new MonotonePolygon(point, bufferPoints.back(), rightPoint);
                        polygonsHash.insertMulti(rightPoint, polygon);
                        polygonsHash.insertMulti(bufferPoints.back(), polygon);
                    } else {
                        addPoint(firstPolygon, point, bufferPoints.back(), writer);
                        if(!firstPolygon->requiresTheLastPoint())
                            polygonsHash.insertMulti(bufferPoints.back(), firstPolygon);

                        MonotonePolygon* polygon = new MonotonePolygon(point, leftPoint, bufferPoints.front());
                        polygonsHash.insertMulti(leftPoint, polygon);
                        polygonsHash.insertMulti(bufferPoints.front(), polygon);
                    }
                } else {
                    /// У нас гарантированно есть два боковых полигона, но есть и
                    /// особая, крайне неприятная ситуация, когда у точка является концом для
                    /// одного ребра(нескольких ребер) и началом для других.
                    /// Тогда у нас появляется дополнительный полигон
                    if(points[point].y > points[rightPoint].y || points[point].y > points[leftPoint].y) {
                        /// У нас есть дополнительный полигон
                        if(exchangeRequired(point, leftPoint, rightPoint))
                            std::swap(leftPoint, rightPoint);
                        if(exchangeRequired(point, firstPolygon, secondPolygon))
                            std::swap(firstPolygon, secondPolygon);
                        if(!exchangeRequired(point, rightPoint, bufferPoints.front())) {
                            /// Ребро(-а) находится правее правой точки
                            MonotonePolygon* polygon = new MonotonePolygon(point, rightPoint, bufferPoints.front());
                            polygonsHash.insertMulti(rightPoint, polygon);
                            polygonsHash.insertMulti(bufferPoints.front(), polygon);

                            addPoint(firstPolygon, point, leftPoint, writer);
                            addPoint(secondPolygon, point, bufferPoints.back(), writer);
                            if(!firstPolygon->requiresTheLastPoint())
                                polygonsHash.insertMulti(leftPoint, firstPolygon);
                            if(!secondPolygon->requiresTheLastPoint())
                                polygonsHash.insertMulti(bufferPoints.back(), secondPolygon);
                        } else {
                            /// Ребро(-а) находится левее левой точки
                            MonotonePolygon* polygon = new MonotonePolygon(point, leftPoint, bufferPoints.back());
                            polygonsHash.insertMulti(leftPoint, polygon);
                            polygonsHash.insertMulti(bufferPoints.back(), polygon);

                            addPoint(firstPolygon, point, bufferPoints.front(), writer);
                            addPoint(secondPolygon, point, rightPoint, writer);
                            if(!firstPolygon->requiresTheLastPoint())
                                polygonsHash.insertMulti( bufferPoints.front(), firstPolygon);
                            if(!secondPolygon->requiresTheLastPoint())
                                polygonsHash.insertMulti(rightPoint, secondPolygon);
                        }
                    } else {
                        if(bufferPoints.size() > 1 && exchangeRequired(point, firstPolygon, secondPolygon))
                            std::swap(firstPolygon, secondPolygon);

                        addPoint(firstPolygon, point, bufferPoints.front(), writer);
                        addPoint(secondPolygon, point, bufferPoints.back(), writer);
                        if(!firstPolygon->requiresTheLastPoint())
                            polygonsHash.insertMulti(bufferPoints.front(), firstPolygon);
                        if(!secondPolygon->requiresTheLastPoint())
                            polygonsHash.insertMulti(bufferPoints.back(), secondPolygon);
                    }
                }
            }
        } else {
            /// Если нет доп. ребер
            if(firstPolygon != nullptr) {
                if(secondPolygon == nullptr) {
                    unsigned predPoint = firstPolygon->giveThePreviousPoint(point);
                    if(predPoint == leftPoint)
                        std::swap(leftPoint, rightPoint);
                    else {
                        if(predPoint != rightPoint) {
                            /// Один из боковых полигонов был удален
                            if(exchangeRequired(point, leftPoint, rightPoint))
                                std::swap(leftPoint, rightPoint);
                            if(!exchangeRequired(point, leftPoint, predPoint)) {
                                /// Полигон находиться правее leftPoint, поэтому следующая точка - right
                                leftPoint = rightPoint;
                            }
                        }
                    }
                    addPoint(firstPolygon, point, leftPoint, writer);
                    if(!firstPolygon->requiresTheLastPoint())
                        polygonsHash.insertMulti(leftPoint, firstPolygon);
                } else {
                    if(exchangeRequired(point, firstPolygon, secondPolygon))
                        std::swap(firstPolygon, secondPolygon);
                    if(exchangeRequired(point, leftPoint, rightPoint))
                         std::swap(leftPoint, rightPoint);
                    addPoint(firstPolygon, point, leftPoint, writer);
                    addPoint(secondPolygon, point, rightPoint, writer);
                    if(!firstPolygon->requiresTheLastPoint())
                        polygonsHash.insertMulti(leftPoint, firstPolygon);
                    if(!secondPolygon->requiresTheLastPoint())
                        polygonsHash.insertMulti(rightPoint, secondPolygon);
                }
            } else {
                /// Если мы нашли хотя бы один полигон, но это не боковая сторона
                /// то тогда мы на end точке, поэтому создавать новый полигон не надо
                if(!findPolygon) {
                    MonotonePolygon* polygon = new MonotonePolygon(point, leftPoint, rightPoint);
                    polygonsHash.insertMulti(leftPoint, polygon);
                    polygonsHash.insertMulti(rightPoint, polygon);
                }
            }
        }
    }
}

//-----------------------------------------------------
//-----------------------------------------------------
//-----------------------------------------------------
//-----------------------------------------------------

/*
struct MonotonePolygon_ {
    unsigned afterTheEnd, beforeTheBeginning; /// Следующие точки
    QStack<unsigned> stack; /// Стек вершин, которые не были триангулированы

    explicit MonotonePolygon_(unsigned point, unsigned next_1, unsigned next_2) {
        afterTheEnd = next_1;
        beforeTheBeginning = next_2;
        stack.push(point);
    }

    inline bool requiresTheLastPoint() const {
        return afterTheEnd == beforeTheBeginning;
    }

    inline bool isLastPoint(unsigned point) {
        return afterTheEnd == point && beforeTheBeginning == point;
    }

    inline bool isNextPoint(unsigned point) {
        return afterTheEnd == point || beforeTheBeginning == point;
    }

    inline unsigned giveThePreviousPoint(unsigned point) {
        if(afterTheEnd == point) {
            return stack.last(); // <=> stack.top()
        } else {
            return stack.first();
        }
    }

};


bool isClockWise(Vector2D left, Vector2D center, Vector2D rigth) {
    Vector2D a, b;
    a.x = center.x - left.x;
    b.x = center.x - rigth.x;
    a.y = center.y - left.y;
    b.y = center.y - rigth.y;
    return (a.x * b.y - a.y * b.x) < 0;
}

/// На счет инверсии массива мы добиваемся того,
/// что обхода по часовой стрелке можно добиться
/// за счет возрастания значения индекса
TriangleIndexes normalize(TriangleIndexes sourse, unsigned shift = 0) {
    if(sourse.first > sourse.second) {
        std::swap(sourse.first, sourse.second);
    }
    if(sourse.second > sourse.third) {
        std::swap(sourse.second, sourse.third);
    }
    if(sourse.first > sourse.second) {
        std::swap(sourse.first, sourse.second);
    }
    sourse.first += shift;
    sourse.second += shift;
    sourse.third += shift;
    return sourse;
}

void MonotoneMethod_GeometryFactory::createIndexesAttribute(QGeometry* geometry, Polyhedron* polyhedron) {
    int shift = 0;
    int countPoints = 0;

    /// Буфер, хранящий номера точек
    QByteArray bufferIndexes;

    /// points выносим выше, чтобы уменьшить накладные расходы
    std::vector<Vector2D> points;
    std::vector<unsigned> pointsIndexes;

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
            /// Резервируем место под все точки полигона
            points.resize(polygon->size());
            pointsIndexes.resize(polygon->size());
            for(unsigned i = 0; i != pointsIndexes.size(); ++i)
                pointsIndexes[i] = i;

            /// Для определения плоскости, на которую сделаем проекцию
            double x = std::abs(polygon->plane().A);
            double y = std::abs(polygon->plane().B);
            double z = std::abs(polygon->plane().C);

            std::function<Vector2D(unsigned)> selector;

            /// Интересный факт
            /// Для сохранения направления обхода нужно инвертировать
            /// одну из осей, если направление нормали не совпадает с нормалью плоскости
            if(x > y && x > z) {
                // (0, 1, 0) x (0, 0, 1) = (1, 0, 0)
                if(polygon->plane().A > 0) {
                    selector = [polygon] (unsigned i) -> Vector2D {
                        Vertex v = polygon->vertex(i);
                        return Vector2D{v.y, v.z};
                    };
                } else {
                    selector = [polygon] (unsigned i) -> Vector2D {
                        Vertex v = polygon->vertex(i);
                        return Vector2D{v.z, v.y};
                    };
                }
            } else if(y > z) {
                // (0, 0, 1) x (1, 0, 0) = (0, 1, 0)
                if(polygon->plane().B > 0) {
                    selector = [polygon] (unsigned i) -> Vector2D {
                        Vertex v = polygon->vertex(i);
                        return Vector2D{v.z, v.x};
                    };
                } else {
                    selector = [polygon] (unsigned i) -> Vector2D {
                        Vertex v = polygon->vertex(i);
                        return Vector2D{v.x, v.z};
                    };
                }
            } else {
                // (1, 0, 0) x (0, 1, 0) = (0, 0, 1)
                if(polygon->plane().C > 0) {
                    selector = [polygon] (unsigned i) -> Vector2D {
                        Vertex v = polygon->vertex(i);
                        return Vector2D{v.x, v.y};
                    };
                } else {
                    selector = [polygon] (unsigned i) -> Vector2D {
                        Vertex v = polygon->vertex(i);
                        return Vector2D{v.y, v.x};
                    };
                }
            }

            if(polygon->isClockwise()) {
                for(unsigned i = 0; i != points.size(); ++i) {
                    points[i] = selector(i);
                }
            } else {
                for(unsigned i = 0; i != points.size(); ++i) {
                    points[i] = selector(points.size() - i - 1);
                }
            }

            Vector2D* dataPtr = points.data();

            /// Для использования заметающей прямой, сортируем точки по невозрастанию 'y'
            std::sort(pointsIndexes.begin(), pointsIndexes.end(), [dataPtr](unsigned p1, unsigned p2) -> bool {
                if(dataPtr[p1].y == dataPtr[p2].y)
                    return dataPtr[p1].x < dataPtr[p2].x;
                else
                    return dataPtr[p1].y > dataPtr[p2].y;
            });

            QLinkedList<Edge> edges;
            std::vector<Edge> appendedEdges;

            auto appendEdge = [dataPtr, &edges] (Edge edge) {
                auto iter = edges.begin();
                while(iter != edges.end() && dataPtr[iter->bottom].y < dataPtr[edge.bottom].y) ++iter;
                edges.insert(iter, edge);
            };

            auto clearEdges = [dataPtr, &edges] (unsigned p) {
                auto min_y = dataPtr[p].y;
                while(!edges.isEmpty() && dataPtr[edges.back().bottom].y >= min_y) edges.removeLast();
            };

            auto isBelow = [dataPtr](unsigned below, unsigned upper) {
                if(dataPtr[below].y == dataPtr[upper].y) {
                    return dataPtr[below].x > dataPtr[upper].x;
                } else {
                    return dataPtr[below].y < dataPtr[upper].y;
                }
            };

            for(unsigned indexPoint = 0; indexPoint != pointsIndexes.size(); ++indexPoint)
            {
                unsigned point = pointsIndexes[indexPoint];
                /// Это не правая и левая точка по x или y, это правая и левая по обходу
                unsigned leftPoint = (point + pointsIndexes.size() - 1) % pointsIndexes.size();
                unsigned rightPoint = (point + 1) % pointsIndexes.size();
                bool leftBelow = isBelow(leftPoint, point);  //dataPtr[leftPoint].y < dataPtr[point].y;
                bool rightBelow = isBelow(rightPoint, point); //dataPtr[rightPoint].y <= dataPtr[point].y;
                bool clockWise = isClockWise(dataPtr[leftPoint], dataPtr[point], dataPtr[rightPoint]);
                /// Очищаем список ребер
                clearEdges(point);
                /// Если угол больше 180 градусов
                if(!clockWise) {
                    if(rightBelow == leftBelow) {
                        Edge leftEdge, rightEdge;
                        bool findLeftEdge = false, findRightEdge = false;
                        {
                            Vector2D current = dataPtr[point];
                            float left_x = -std::numeric_limits<float>::max();
                            float rigth_x = std::numeric_limits<float>::max();
                            /// Ищем ближайшие ребра
                            for(auto iter = edges.begin(); iter != edges.end(); ++iter) {
                                if(iter->top == point || iter->bottom == point) continue;

                                Vector2D top = dataPtr[iter->top];
                                Vector2D bottom = dataPtr[iter->bottom];

                                float intersection = top.x + (bottom.x - top.x) / (bottom.y - top.y) * (current.y - top.y);
                                if(intersection < current.x && intersection > left_x) {
                                    left_x = intersection;
                                    leftEdge = *iter;
                                    findLeftEdge = true;
                                }
                                if(intersection > current.x && intersection < rigth_x) {
                                    rigth_x = intersection;
                                    rightEdge = *iter;
                                    findRightEdge = true;
                                }
                            }
                        }
                        if(findLeftEdge && findRightEdge) {
                            if(rightBelow) { // split вершина (rigthBelow && leftBelow)
                                unsigned minPoint;
                                if(dataPtr[leftEdge.top].y < dataPtr[rightEdge.top].y)
                                    minPoint = leftEdge.top;
                                else
                                    minPoint = rightEdge.top;
                                /// Ищем точку лежащую между ребрами
                                unsigned max = indexPoint - 1;
                                for(; pointsIndexes[max] != minPoint; --max)
                                {
                                    Vector2D top, bottom, current = dataPtr[pointsIndexes[max]];
                                    if(current.y == dataPtr[point].y) continue;
                                    top = dataPtr[pointsIndexes[leftEdge.top]];
                                    bottom = dataPtr[pointsIndexes[leftEdge.bottom]];
                                    float intersection;
                                    /// Если вершина левее левого ребра
                                    intersection = top.x + (top.x - bottom.x) / (top.y - bottom.y) * (top.y - current.y);
                                    if(intersection > current.x) continue;

                                    top = dataPtr[pointsIndexes[rightEdge.top]];
                                    bottom = dataPtr[pointsIndexes[rightEdge.bottom]];
                                    intersection = top.x + (top.x - bottom.x) / (top.y - bottom.y) * (top.y - current.y);
                                    /// Или правее правого ребра
                                    if(intersection < current.x) continue;

                                    appendedEdges.push_back(Edge{pointsIndexes[max], point});
                                    break;
                                }
                                if(pointsIndexes[max] == minPoint) {
                                    appendedEdges.push_back(Edge{minPoint, point});
                                }
                            } else { // merge вершина !(rigthBelow || leftBelow)
                                unsigned minPoint;
                                if(dataPtr[leftEdge.bottom].y > dataPtr[rightEdge.bottom].y)
                                    minPoint = leftEdge.bottom;
                                else
                                    minPoint = rightEdge.bottom;
                                /// Ищем точку лежащую между ребрами
                                unsigned max = indexPoint + 1;
                                for(; pointsIndexes[max] != minPoint; ++max)
                                {
                                    Vector2D top, bottom, current = dataPtr[pointsIndexes[max]];
                                    top = dataPtr[pointsIndexes[leftEdge.top]];
                                    bottom = dataPtr[pointsIndexes[leftEdge.bottom]];
                                    float intersection;
                                    /// Если вершина левее левого ребра
                                    intersection = top.x + (top.x - bottom.x) / (top.y - bottom.y) * (top.y - current.y);
                                    if(intersection > current.x) continue;

                                    top = dataPtr[pointsIndexes[rightEdge.top]];
                                    bottom = dataPtr[pointsIndexes[rightEdge.bottom]];
                                    intersection = top.x + (top.x - bottom.x) / (top.y - bottom.y) * (top.y - current.y);
                                    /// Или правее правого ребра
                                    if(intersection < current.x) continue;

                                    appendedEdges.push_back(Edge{point, pointsIndexes[max]});
                                    break;
                                }
                                if(pointsIndexes[max] == minPoint) {
                                    appendedEdges.push_back(Edge{point, minPoint});
                                }
                            }
                        }
                    }
                }
                /// Добавляем новые ребра
                if(rightBelow) {
                    appendEdge(Edge{point, rightPoint});
                }
                if(leftBelow) {
                    appendEdge(Edge{point, leftPoint});
                }
            }

            /// Сортируем добаленные грани по не убыванию верхней вершины
            /// Если общая верхняя вершина, то сортирует по x нижней части ребра
            std::sort(appendedEdges.begin(), appendedEdges.end(), [dataPtr](Edge e1, Edge e2) -> bool {
                if(e1.top == e2.top)
                    return dataPtr[e1.bottom].x < dataPtr[e2.bottom].x;
                else
                    return dataPtr[e1.top].y > dataPtr[e2.top].y;
            });



            Edge *edgeBegin = appendedEdges.data();
            Edge *edgeEnd = edgeBegin + appendedEdges.size();
            QHash<unsigned, MonotonePolygon_*> hash; // Ключ - ожидаемая точка

            auto createNewPolygon = [&hash] (unsigned point, unsigned next_1, unsigned next_2) -> void {
                MonotonePolygon_* result = new MonotonePolygon_(point, next_1, next_2);
                hash.insertMulti(next_1, result);
                hash.insertMulti(next_2, result);
            };

            auto addPointToPolygon = [dataPtr, shift, &bufferIndexes] (MonotonePolygon_* polygon, unsigned point, unsigned nextPoint) -> bool {
                if(polygon->beforeTheBeginning != point && polygon->afterTheEnd != point) {
                    return false;
                }
                if(polygon->stack.size() < 2) {
                    polygon->stack.push(point);
                    if(polygon->beforeTheBeginning == point){
                        polygon->beforeTheBeginning = polygon->afterTheEnd;
                    }
                    polygon->afterTheEnd = nextPoint;
                }
                else {
                    if(polygon->beforeTheBeginning == point) {
                        // Если точка не является продолжением дуги или является последней точкой в полигоне
                        unsigned top = polygon->stack.pop(); // Запоминаем точку с вершины стека
                        TriangleIndexes triangle;
                        triangle.first = point;
                        triangle.third = top;
                        while(!polygon->stack.empty()) {
                            triangle.second = triangle.third;
                            triangle.third = polygon->stack.pop();
                            TriangleIndexes result = normalize(triangle, shift);
                            bufferIndexes.append((char*)&result, sizeof(unsigned short) * 3);
                        }
                        /// Для последней точки это ничего не значит,
                        /// но на дальнейшую работу это не повлияет (объект будет удален),
                        /// а делать отдельную проверку для редкого события такое себе
                        polygon->stack.push(point);
                        polygon->stack.push(top);
                        polygon->beforeTheBeginning = nextPoint;
                    } else {
                        // Если точка является продолжением дуги
                        TriangleIndexes triangle;
                        triangle.first = point;
                        triangle.second = polygon->stack.pop(); // Запоминаем точку с вершины стека
                        triangle.third = polygon->stack.top(); // берем еще одну точку из стека
                        while(true) {
                            TriangleIndexes result = normalize(triangle, shift);
                            if(isClockWise(dataPtr[result.first], dataPtr[result.second], dataPtr[result.third])) {
                                bufferIndexes.append((char*)&result, sizeof(unsigned short) * 3);
                                triangle.second = triangle.third;
                                polygon->stack.pop();
                                if(polygon->stack.empty()) break;
                                triangle.third = polygon->stack.top();
                            } else break;
                        }
                        polygon->stack.push(triangle.second);
                        polygon->stack.push(triangle.first);
                        polygon->afterTheEnd = nextPoint;
                    }
                }
                return true;
            };

            auto addToHash = [&hash] (MonotonePolygon_* polygon, unsigned nextPoint) {
                if(!polygon->requiresTheLastPoint())
                    hash.insertMulti(nextPoint, polygon);
            };

            for(unsigned indexPoint = 0; indexPoint != pointsIndexes.size(); ++indexPoint)
            {
                unsigned point = pointsIndexes[indexPoint];
                unsigned leftPoint = (point + pointsIndexes.size() - 1) % pointsIndexes.size();
                unsigned rightPoint = (point + 1) % pointsIndexes.size();

                auto polygonsRange = hash.equal_range(point);
                /// Если точка не принадлежит ни одному полигону
                /// то она является точкой start
                if(polygonsRange.first == hash.end()) {
                    if(edgeBegin != edgeEnd && point == edgeBegin->top)
                    {
                        /// Если ребер несколько
                        if(edgeBegin + 1 != edgeEnd && point == (edgeBegin + 1)->top) {
                            float lx, rx;
                            Vector2D lp = dataPtr[leftPoint];
                            Vector2D rp = dataPtr[rightPoint];
                            Vector2D p = dataPtr[point];

                            if(lp.y == p.y || rp.y == p.y) { // Обработка исключительного условия
                                lx = lp.x;
                                rx = rp.x;
                            } else {
                                lx = (lp.x - p.x) / std::abs(lp.y - p.y);
                                rx = (rp.x - p.x) / std::abs(rp.y - p.y);
                            }
                            if(rx < lx) {
                                std::swap(leftPoint, rightPoint);
                            }
                        }

                        while(edgeBegin != edgeEnd && point == edgeBegin->top) {
                            createNewPolygon(point, leftPoint, edgeBegin->bottom);
                            leftPoint = edgeBegin->bottom;
                            ++edgeBegin;
                        }
                    }
                    createNewPolygon(point, leftPoint, rightPoint);
                } else {
                    /// Иначе она может быть
                    /// 1) Start - если есть несколько ребер, выходящих из точки
                    /// 2) Промежуточной
                    /// 3) End точкой

                    auto startEdge = edgeBegin;
                    while(edgeBegin != edgeEnd && point == edgeBegin->top) {
                        ++edgeBegin;
                    }
                    auto endEdge = edgeBegin;

                    /// Может быть не больше двух боковых полигонов
                    MonotonePolygon_ *firstPolygon = nullptr, *secondPolygon = nullptr;

                    while(polygonsRange.first != polygonsRange.second) {
                        MonotonePolygon_* polygon = polygonsRange.first.value();
                        if(polygon->requiresTheLastPoint()) {
                            /// значение next не имеет значения
                            /// поскольку этот многоугольник мы удалим
                            addPointToPolygon(polygon, point, 0);
                            /// Удаляем многоугольник, поскольку мы больше не будем его обрабатывать
                            delete polygon;
                        } else {
                            if(firstPolygon == nullptr) {
                                firstPolygon = polygon;
                            } else {
                                secondPolygon = polygon;
                            }
                        }
                        ++polygonsRange.first;
                    }
                    hash.remove(point);

                    /// Если есть добавленные ребра
                    if(startEdge != endEdge) {
                        --endEdge;
                        /// Если ребер больше двух
                        if(startEdge != endEdge) {
                            float lx, rx;
                            Vector2D lp = dataPtr[leftPoint];
                            Vector2D rp = dataPtr[rightPoint];
                            Vector2D p = dataPtr[point];

                            if(lp.y == p.y || rp.y == p.y) { // Обработка исключительного условия
                                lx = lp.x;
                                rx = rp.x;
                            } else {
                                lx = (lp.x - p.x) / std::abs(lp.y - p.y);
                                rx = (rp.x - p.x) / std::abs(rp.y - p.y);
                            }
                            if(rx < lx) {
                                std::swap(leftPoint, rightPoint);
                            }
                        }
                        if(firstPolygon == nullptr) {
                            /// оба боковых полигона не созданы
                            createNewPolygon(point, leftPoint, startEdge->bottom);
                            createNewPolygon(point, rightPoint, endEdge->bottom);
                        } else {
                            if(secondPolygon == nullptr) {
                                if(leftPoint == firstPolygon->giveThePreviousPoint(point)) {
                                    addPointToPolygon(firstPolygon, point, startEdge->bottom);
                                    addToHash(firstPolygon, startEdge->bottom);
                                    createNewPolygon(point, rightPoint, endEdge->bottom);
                                } else {
                                    addPointToPolygon(firstPolygon, point, endEdge->bottom);
                                    addToHash(firstPolygon, endEdge->bottom);
                                    createNewPolygon(point, leftPoint, startEdge->bottom);
                                }
                            } else {
                                MonotonePolygon_ *leftPolygon = firstPolygon, *rightPolygon = secondPolygon;
                                if(leftPoint != firstPolygon->giveThePreviousPoint(point)) {
                                    std::swap(leftPolygon, rightPolygon);
                                }
                                addPointToPolygon(leftPolygon, point, startEdge->bottom);
                                addPointToPolygon(rightPolygon, point, endEdge->bottom);
                                addToHash(leftPolygon, startEdge->bottom);
                                addToHash(rightPolygon, endEdge->bottom);
                            }
                        }
                        /// Создаем полигона, находящиеся между двумя добавленными ребрами
                        while(edgeBegin != edgeEnd && point == edgeBegin->top) {
                            createNewPolygon(point, leftPoint, edgeBegin->bottom);
                            leftPoint = edgeBegin->bottom;
                        }
                    } else {
                        /// Если нет добавленных ребер
                        /// И это не последняя точка
                        /// TODO доделать выбор стороны
                        if(firstPolygon != nullptr) {
                            if(secondPolygon != nullptr) {
                                Vector2D p = dataPtr[point];
                                {
                                    Vector2D fp, sp;
                                    if(firstPolygon->stack.top() == secondPolygon->stack.top()) {
                                        if(firstPolygon->stack.first() == secondPolygon->stack.first()) {
                                            if(firstPolygon->afterTheEnd == secondPolygon->afterTheEnd) {
                                                fp = dataPtr[firstPolygon->beforeTheBeginning];
                                                sp = dataPtr[secondPolygon->beforeTheBeginning];
                                            } else {
                                                fp = dataPtr[firstPolygon->afterTheEnd];
                                                sp = dataPtr[secondPolygon->afterTheEnd];
                                            }
                                        } else {
                                            fp = dataPtr[firstPolygon->stack.first()];
                                            sp = dataPtr[secondPolygon->stack.first()];
                                        }
                                    } else {
                                        fp = dataPtr[firstPolygon->stack.top()];
                                        sp = dataPtr[secondPolygon->stack.top()];
                                    }

                                    float fx, sx;
                                    if(fp.y == p.y || sp.y == p.y) { // Обработка исключительной ситуации
                                        fx = fp.x;
                                        sx = sp.x;
                                    } else {
                                        fx = (fp.x - p.x) / std::abs(fp.y - p.y);
                                        sx = (sp.x - p.x) / std::abs(sp.y - p.y);
                                    }

                                    if(fx > sx) {
                                        /// Делаем так, чтобы firstPolygon лежал левее
                                        std::swap(firstPolygon, secondPolygon);
                                    }
                                }

                                float lx, rx;
                                Vector2D lp = dataPtr[leftPoint];
                                Vector2D rp = dataPtr[rightPoint];

                                if(lp.y == p.y || rp.y == p.y) { // Обработка исключительного условия
                                    lx = lp.x;
                                    rx = rp.x;
                                } else {
                                    lx = (lp.x - p.x) / std::abs(lp.y - p.y);
                                    rx = (rp.x - p.x) / std::abs(rp.y - p.y);
                                }

                                if(rx < lx) {
                                    std::swap(leftPoint, rightPoint);
                                }

                                addPointToPolygon(firstPolygon, point, leftPoint);
                                addToHash(firstPolygon, leftPoint);

                                addPointToPolygon(secondPolygon, point, rightPoint);
                                addToHash(secondPolygon, rightPoint);
                            } else {
                                if(leftPoint == firstPolygon->giveThePreviousPoint(point)) {
                                    addPointToPolygon(firstPolygon, point, rightPoint);
                                    addToHash(firstPolygon, rightPoint);
                                } else {
                                    addPointToPolygon(firstPolygon, point, leftPoint);
                                    addToHash(firstPolygon, leftPoint);
                                }
                            }
                        }
                    }
                }
            }
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
*/
}
