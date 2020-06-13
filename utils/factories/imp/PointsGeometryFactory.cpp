#include "PointsGeometryFactory.h"
namespace bstu {

QGeometryRenderer* PointsGeometryFactory::create(Polyhedron* polyhedron) {
    QGeometry *geometry = new Qt3DRender::QGeometry();
    createVertexAttribute(geometry, polyhedron);
    QGeometryRenderer *renderer = new QGeometryRenderer();
    renderer->setGeometry(geometry);
    renderer->setPrimitiveType(QGeometryRenderer::Points);
    return renderer;
}

void PointsGeometryFactory::createVertexAttribute(QGeometry* geometry, Polyhedron* polyhedron) {
    /// Создаем атрибут отображения геометрии (позиции точек в пространстве)
    Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute(geometry);
    /// Добавляем в геометрию объекта наш атрибут
    geometry->addAttribute(positionAttribute);

    QByteArray bufferVertex;

    for(int i = 0; i < polyhedron->size(); ++i)
    {
        Polygon* polygon = polyhedron->polygon(i);
        for(int j = 0; j < polygon->size(); ++j)
        {
            Vertex tmp = polygon->vertex(j);
            bufferVertex.append((char*)&tmp, sizeof(Vertex));
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
    /// Указываем количество точек
    positionAttribute->setCount(bufferVertex.size() / sizeof(Vertex));
}

void PointsGeometryFactory::createIndexesAttribute(QGeometry* geometry, Polyhedron* polyhedron) {
    /// Буфер, хранящий номера точек
    QByteArray bufferIndexes;
    struct { unsigned short first, second, third; } triangle;
    for(int i = 0; i < polyhedron->size(); ++i)
    {
        Polygon* polygon = polyhedron->polygon(i);
        for(unsigned j = 0; j < polygon->size(); ++j)
            bufferIndexes.append((char*)&j, sizeof(unsigned));
    }
    /// Создаем буфер, связанный с геометрией объекта
    Qt3DRender::QBuffer *indexesBuf = new Qt3DRender::QBuffer(geometry);
    /// Передаем в него точки
    indexesBuf->setData(bufferIndexes);
    /// Создаем атрибут отображения геометрии (номера точек)
    QAttribute *indexAttribute = new QAttribute(geometry);
    /// Указываем, какой класс является базовым для номеров
    indexAttribute->setVertexBaseType(QAttribute::UnsignedInt);
    /// Указываем, что атрибут отвечает за номера точек
    indexAttribute->setAttributeType(QAttribute::IndexAttribute);
    /// Передаем точки
    indexAttribute->setBuffer(indexesBuf);
    /// Указываем число точек
    indexAttribute->setCount(bufferIndexes.size() / sizeof(unsigned));
    /// Добавляем в геометрию объекта наш атрибут
    geometry->addAttribute(indexAttribute);
}
}

