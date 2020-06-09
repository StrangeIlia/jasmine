#include "SimpleGeometryFactory.h"

л
namespace bstu {

QGeometryRenderer* SimpleGeometryFactory::create(Polyhedron* polyhedron) {
    QGeometry *geometry = new Qt3DRender::QGeometry();
    createVertexAttribute(geometry, polyhedron);
    createIndexesAttribute(geometry, polyhedron);
    QGeometryRenderer *renderer = new QGeometryRenderer();
    renderer->setGeometry(geometry);
    renderer->setPrimitiveType(QGeometryRenderer::Triangles);
    return renderer;
}

void SimpleGeometryFactory::createVertexAttribute(QGeometry* geometry, Polyhedron* polyhedron) {
    /// Создаем атрибут отображения геометрии (позиции точек в пространстве)
    Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute(geometry);
    /// Создаем атрибут отображения геометрии (нормали точек в пространстве)
    Qt3DRender::QAttribute *normalAttribute = new Qt3DRender::QAttribute(geometry);
    /// Добавляем в геометрию объекта наш атрибут
    geometry->addAttribute(positionAttribute);
    geometry->addAttribute(normalAttribute);

    int countPoints = 0;
    QByteArray bufferVertex;
    int stride = sizeof(Vertex) * 2;
    for(int i = 0; i < polyhedron->size(); ++i)
    {
        Polygon* polygon = polyhedron->polygon(i);
        Plane plane = polygon->plane();
        Vertex normal;
        normal.x = plane.A;
        normal.y = plane.B;
        normal.z = plane.C;
        int additionSize = polygon->size() * stride;
        bufferVertex.reserve(bufferVertex.size() + additionSize);
        countPoints += polygon->size();
        if(polygon->isClockwise())
        {
            for(int j = 0; j < polygon->size(); ++j)
            {
                Vertex tmp = polygon->vertex(j);
                bufferVertex.append((char*)&tmp, sizeof(Vertex));
                bufferVertex.append((char*)&normal, sizeof(Vertex));
            }
        }
        else
        {
            for(int j = polygon->size() - 1; j >= 0; --j)
            {
                Vertex tmp = polygon->vertex(j);
                bufferVertex.append((char*)&tmp, sizeof(Vertex));
                bufferVertex.append((char*)&normal, sizeof(Vertex));
            }
        }
    }

    /// Создаем буфер, связанный с геометрией объекта
    Qt3DRender::QBuffer *buf = new Qt3DRender::QBuffer(geometry);
    /// Передаем в него точки
    buf->setData(bufferVertex);

    /// Указываем название атрибута (по умолчанию для позиций точек)
    positionAttribute->setName(QAttribute::defaultPositionAttributeName());
    /// Указываем, какой класс является базовым для Vertex
    positionAttribute->setVertexBaseType(QAttribute::Float);
    /// Указываем число измерений в Vertex
    positionAttribute->setVertexSize(3);
    /// Указываем, что атрибут отвечает за позиции точек в пространстве
    positionAttribute->setAttributeType(QAttribute::VertexAttribute);
    /// Передаем точки
    positionAttribute->setBuffer(buf);
    /// Указываем, какой интервал между точками
    positionAttribute->setByteStride(stride);
    /// Указываем сдвиг в байтах в буфере
    normalAttribute->setByteOffset(0);
    /// Указываем количество точек
    positionAttribute->setCount(countPoints);

    /// Указываем название атрибута (по умолчанию для нормалей точек)
    normalAttribute->setName(QAttribute::defaultNormalAttributeName());
    /// Указываем, какой класс является базовым для Vertex
    normalAttribute->setVertexBaseType(QAttribute::Float);
    /// Указываем число измерений в Vertex
    normalAttribute->setVertexSize(3);
    /// Указываем, что атрибут отвечает за позиции точек в пространстве
    normalAttribute->setAttributeType(QAttribute::VertexAttribute);
    /// Передаем точки
    normalAttribute->setBuffer(buf);
    /// Указываем, какой интервал между точками
    normalAttribute->setByteStride(stride);
    /// Указываем сдвиг в байтах в буфере
    normalAttribute->setByteOffset(sizeof(Vertex));
    /// Указываем количество точек
    normalAttribute->setCount(countPoints);
}

void SimpleGeometryFactory::createIndexesAttribute(QGeometry* geometry, Polyhedron* polyhedron) {
    int index = 0;
    int countPoints = 0;
    /// Буфер, хранящий номера точек
    QByteArray bufferIndexes;
    struct { unsigned short first, second, third; } triangle;
    for(int i = 0; i < polyhedron->size(); ++i)
    {
        triangle.first = index;
        triangle.third = triangle.first + 1;
        Polygon* polygon = polyhedron->polygon(i);
        int additionSize = (polygon->size() - 2) * sizeof(triangle);
        bufferIndexes.reserve(bufferIndexes.size() + additionSize);
        for(unsigned j = 2; j < polygon->size(); ++j)
        {
            triangle.second = triangle.third;
            triangle.third = triangle.second + 1;
            bufferIndexes.append((char*)&triangle, sizeof(triangle));
        }
        index += polygon->size();
        countPoints += (polygon->size() - 2) * 3;
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

