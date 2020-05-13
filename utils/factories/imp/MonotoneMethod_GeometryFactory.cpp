#include "MonotoneMethod_GeometryFactory.h"

namespace bstu {

QGeometryRenderer* MonotoneMethod_GeometryFactory::create(Polyhedron* polyhedron) {
    QGeometry *geometry = new Qt3DRender::QGeometry();
    SimpleGeometryFactory::createVertexAttribute(geometry, polyhedron);
    MonotoneMethod_GeometryFactory::createIndexesAttribute(geometry, polyhedron);
    QGeometryRenderer *renderer = new QGeometryRenderer();
    renderer->setGeometry(geometry);
    renderer->setPrimitiveType(QGeometryRenderer::Triangles);
    //renderer->setPrimitiveType(QGeometryRenderer::Lines);
    return renderer;
}

struct Vector2D { float x, y; };
struct Edge { unsigned top; unsigned bottom; };
struct Triangle { unsigned short first, second, third; };

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
Triangle normalize(Triangle sourse, unsigned shift = 0) {
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
                while(!edges.isEmpty() && dataPtr[edges.back().bottom].y > min_y) edges.removeLast();
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
                        Edge leftEdge, rigthEdge;
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
                                }
                                if(intersection > current.x && intersection < rigth_x) {
                                    rigth_x = intersection;
                                    rigthEdge = *iter;
                                }
                            }
                        }

                        if(rightBelow) { // split вершина (rigthBelow && leftBelow)
                            unsigned minPoint;
                            if(dataPtr[leftEdge.top].y < dataPtr[rigthEdge.top].y)
                                minPoint = leftEdge.top;
                            else
                                minPoint = rigthEdge.top;
                            /// Ищем точку лежащую между ребрами
                            unsigned max = indexPoint - 1;
                            for(; pointsIndexes[max] != minPoint; --max)
                            {
                                Vector2D top, bottom, current = dataPtr[pointsIndexes[max]];
                                top = dataPtr[pointsIndexes[leftEdge.top]];
                                bottom = dataPtr[pointsIndexes[leftEdge.bottom]];
                                float intersection;
                                /// Если вершина левее левого ребра
                                intersection = top.x + (top.x - bottom.x) / (top.y - bottom.y) * (top.y - current.y);
                                if(intersection > current.x) continue;

                                top = dataPtr[pointsIndexes[rigthEdge.top]];
                                bottom = dataPtr[pointsIndexes[rigthEdge.bottom]];
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
                            if(dataPtr[leftEdge.bottom].y > dataPtr[rigthEdge.bottom].y)
                                minPoint = leftEdge.bottom;
                            else
                                minPoint = rigthEdge.bottom;
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

                                top = dataPtr[pointsIndexes[rigthEdge.top]];
                                bottom = dataPtr[pointsIndexes[rigthEdge.bottom]];
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

            struct MonotonePolygon {
                unsigned lastPoint_1, lastPoint_2; /// Предыдущие точки
                unsigned nextPoint_1, nextPoint_2; /// Следующие точки
                QStack<unsigned> stack; /// Стек вершин, которые не были триангулированы

                explicit MonotonePolygon(unsigned point, unsigned next_1, unsigned next_2) {
                    lastPoint_1 = point;
                    lastPoint_2 = point;
                    nextPoint_1 = next_1;
                    nextPoint_2 = next_2;
                    stack.push(point);
                }
            };

            Edge *edgeBegin = appendedEdges.data();
            Edge *edgeEnd = edgeBegin + appendedEdges.size();
            QLinkedList<MonotonePolygon> polygons;          

            auto addPointToPolygon = [dataPtr, shift, &bufferIndexes] (MonotonePolygon& polygon, unsigned point, unsigned nextPoint) -> bool {
                unsigned lastPoint;
                bool isEndPoint = polygon.nextPoint_1 == polygon.nextPoint_2;
                if(polygon.nextPoint_1 == point){
                    lastPoint = polygon.lastPoint_1;
                    polygon.lastPoint_1 = polygon.nextPoint_1;
                    polygon.nextPoint_1 = nextPoint;
                } else if(polygon.nextPoint_2 == point) {
                    lastPoint = polygon.lastPoint_2;
                    polygon.lastPoint_2 = polygon.nextPoint_2;
                    polygon.nextPoint_2 = nextPoint;
                }
                else {
                    return false;
                }
                if(polygon.stack.size() < 2) {
                    polygon.stack.push(point);
                } else {
                    if(lastPoint != polygon.stack.top() || isEndPoint) {
                        // Если точка не является продолжением дуги или является последней точкой в полигоне
                        unsigned top = polygon.stack.pop(); // Запоминаем точку с вершины стека
                        Triangle triangle;
                        triangle.first = point;
                        triangle.third = top;
                        while(!polygon.stack.empty()) {
                            triangle.second = triangle.third;
                            triangle.third = polygon.stack.pop();
                            Triangle result = normalize(triangle, shift);
                            bufferIndexes.append((char*)&result, sizeof(unsigned short) * 3);
                        }
                        /// Для последней точки это ничего не значит,
                        /// но на дальнейшую работу это не повлияет (объект будет удален),
                        /// а делать отдельную проверку для редкого события такое себе
                        polygon.stack.push(point);
                        polygon.stack.push(top);

                    } else {
                        Triangle triangle;
                        triangle.first = point;
                        triangle.second = polygon.stack.pop(); // Запоминаем точку с вершины стека
                        triangle.third = polygon.stack.top(); // берем еще одну точку из стека
                        while(true) {
                            Triangle result = normalize(triangle, shift);
                            if(isClockWise(dataPtr[result.first], dataPtr[result.second], dataPtr[result.third])) {
                                bufferIndexes.append((char*)&result, sizeof(unsigned short) * 3);
                                triangle.second = triangle.third;
                                polygon.stack.pop();
                                if(polygon.stack.empty()) break;
                                triangle.third = polygon.stack.top();
                            } else break;
                        }
                        polygon.stack.push(triangle.second);
                        polygon.stack.push(triangle.first);
                    }
                }
                return true;
            };



            for(unsigned indexPoint = 0; indexPoint != pointsIndexes.size(); ++indexPoint)
            {
                unsigned point = pointsIndexes[indexPoint];
                unsigned leftPoint = (point + pointsIndexes.size() - 1) % pointsIndexes.size();
                unsigned rightPoint = (point + 1) % pointsIndexes.size();
                bool leftBelow = isBelow(leftPoint, point);  //dataPtr[leftPoint].y < dataPtr[point].y;
                bool rightBelow = isBelow(rightPoint, point); //dataPtr[rightPoint].y < dataPtr[point].y;

                /// Если точка соединена с добавленным ребром
                if(edgeBegin != edgeEnd && point == edgeBegin->top) {
                    Edge *leftEdge = edgeBegin;
                    ++edgeBegin;
                    while(edgeBegin != edgeEnd && point == edgeBegin->top) {
                        ++edgeBegin;
                    }
                    Edge *rightEdge = edgeBegin - 1;

                    /// Если ребер несколько, тогда необходимо определить какая
                    /// точка левее, а какая правее по x
                    if(leftEdge != rightEdge) {
                        float lx, rx;
                        Vector2D lp = dataPtr[leftPoint];
                        Vector2D rp = dataPtr[rightPoint];
                        Vector2D p = dataPtr[point];

                        if(lp.y == 0.0f || rp.y == 0.0f) { // Обработка исключительного условия
                            lx = lp.x;
                            rx = rp.x;
                        } else {
                            lx = (lp.x - p.x) / lp.y;
                            rx = (rp.x - p.x) / rp.y;
                        }
                        if(rx < lx) {
                            std::swap(leftPoint, rightPoint);
                            std::swap(leftBelow, rightBelow);
                        }
                    }

                    /// Из условий следует, что для правого и левого многоугольника
                    /// point будет промежуточной стороной, следовательно
                    /// следующая точка будет leftEdge->bottom и rigthEdge->bottom.
                    /// Остальные стороны образуют новые многоугольники, для которых
                    /// point будет start точкой
                    /// Однако, если левая вершина ниже, то нет "левого" многоугольника (он не обязательно слево)
                    /// А если правая вершина ниже, то нет
                    /// Также есть проблема, если есть оба многоугольника
                    if(!(leftBelow || rightBelow)) {
                        /// Поиск многоугольников
                        auto polygonIter = polygons.begin();
                        bool findFirstPolygon = false;
                        while(polygonIter != polygons.end()) {
                            if(polygonIter->nextPoint_1 == point || polygonIter->nextPoint_2 == point) {
                                if(polygonIter->lastPoint_1 == leftPoint || polygonIter->lastPoint_2 == leftPoint) {
                                    addPointToPolygon(*polygonIter, point, leftEdge->bottom);
                                } else {
                                    addPointToPolygon(*polygonIter, point, rightEdge->bottom);
                                }
                                if(findFirstPolygon) break;
                                findFirstPolygon = true;
                            }
                            ++polygonIter;
                        }
                    } else {
                        if(leftBelow && rightBelow) {
                            polygons.append(MonotonePolygon(point, leftPoint, leftEdge->bottom));
                            polygons.append(MonotonePolygon(point, rightBelow, rightEdge->bottom));
                        } else if(leftBelow) {
                            auto polygonIter = polygons.begin();
                            while(polygonIter != polygons.end()) {
                                if(addPointToPolygon(*polygonIter, point, rightEdge->bottom)) {
                                    break;
                                }
                                ++polygonIter;
                            }
                            polygons.append(MonotonePolygon(point, leftPoint, leftEdge->bottom));
                        } else {
                            auto polygonIter = polygons.begin();
                            while(polygonIter != polygons.end()) {
                                if(addPointToPolygon(*polygonIter, point, leftEdge->bottom)) {
                                    break;
                                }
                                ++polygonIter;
                            }
                            polygons.append(MonotonePolygon(point, leftPoint, leftEdge->bottom));
                        }
                    }

                    /// Добавляем новые полигоны
                    while(leftEdge != rightEdge) {
                        polygons.append(MonotonePolygon(point, leftEdge->bottom, (leftEdge + 1)->bottom));
                        ++leftEdge;
                    }

                } else {
                    auto polygonIter = polygons.begin();
                    while(polygonIter != polygons.end()) {
                        if()
                    }

                    if(leftBelow != rightBelow) { /// Промежуточная вершина
                        /// Следующая точка должна быть обязательно не выше
                        unsigned belowPoint = leftBelow ? leftPoint : rightPoint;
                        auto polygonIter = polygons.begin();
                        while(polygonIter != polygons.end()) {
                            if(addPointToPolygon(*polygonIter, point, belowPoint))
                                break;
                            ++polygonIter;
                        }
                    } else if(leftBelow) {
                        /// Если start
                        /// Добавляем новый многоугольник
                        polygons.append(MonotonePolygon(point, leftPoint, rightPoint));
                    } else { /// Если end
                        auto polygonIter = polygons.begin();
                        /// End точка может принадлежать нескольким многоугольникам (точное число неизвестно)
                        /// Поэтому проходим по всем изместным многоуольникам
                        while(polygonIter != polygons.end()) {
                            /// Для end точки polygonIter->nextPoint_1 == polygonIter->nextPoint_2
                            if(polygonIter->nextPoint_1 == point) {
                                /// значение next не имеет значения
                                /// поскольку этот многоугольник мы удалим
                                addPointToPolygon(*polygonIter, point, 0);
                                // Удаляем многоугольник, поскольку мы больше не будем его обрабатывать
                                polygonIter = polygons.erase(polygonIter);
                            } else ++polygonIter;
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

}
