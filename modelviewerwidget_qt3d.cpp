#include "modelviewerwidget_qt3d.h"

namespace bstu {

using namespace Qt3DCore;
using namespace Qt3DInput;
using namespace Qt3DRender;
using namespace Qt3DExtras;

ModelViewerWidget_Qt3D::ModelViewerWidget_Qt3D(QWidget* parent)
{
    ///------------------------------------------------
    /// Отвечает за физ. устройства
    registerAspect(new Qt3DInput::QInputAspect);
    ///------------------------------------------------

    /// Устанавливаем цвет фона
    defaultFrameGraph()->setClearColor(Qt::black);

    /// Напрямую отображать 3D окно нельзя,
    /// поэтому создаем ему оболочку
    _container = QWidget::createWindowContainer(this, parent);

    /// Центральная сущность
    _rootEntity = new Qt3DCore::QEntity;
    setRootEntity(_rootEntity);

    /// Теперь настраиваем камеру
    Qt3DRender::QCamera *cameraEntity = camera();

    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0.0f, 0.0f, -5.0f));
    cameraEntity->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));

    /// Говорим, что камера будет орбитальной
    QOrbitCameraController* orbitCameraController = new QOrbitCameraController(_rootEntity);
    orbitCameraController->setCamera(cameraEntity);
    /// Устанавливаем предел на приближение (на отдаление предела нет)
    orbitCameraController->setZoomInLimit(0.2f);
    _cameraController = orbitCameraController;

    /// Настраиваем освещение
    Qt3DCore::QEntity *lightEntity = new QEntity(_rootEntity);
    Qt3DRender::QPointLight *light = new QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1);
    lightEntity->addComponent(light);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(cameraEntity->position());
    lightEntity->addComponent(lightTransform);
    _sourcesOfLight.push_back(light);
}

ModelViewerWidget_Qt3D::~ModelViewerWidget_Qt3D()
{
    clearEntities();
    clearSourcesOfLight();
    _cameraController->deleteLater();
    _rootEntity->deleteLater();
    _container->deleteLater();
}

void ModelViewerWidget_Qt3D::setTree(Tree* tree)
{
    clearEntities();
    QPhongAlphaMaterial *material = new Qt3DExtras::QPhongAlphaMaterial(_rootEntity);
    /// Указываем цвет материала
    material->setAmbient(QColor::fromRgbF(50.0 / 255.0, 130.0 / 255.0, 200.0 / 255.0, 1));
    //material->setAlpha(1.0f);
    /*material->setAmbient(QColor(28, 61, 136));
    material->setDiffuse(QColor(11, 56, 159));
    material->setSpecular(QColor(10, 67, 199));
    */
    material->setShininess(0.8f);
    material->setAlpha(1.0f);
    for (unsigned i = 0; i != tree->polyhedronsCount(); i++)
    {
        Polyhedron* polyhedron = tree->polyhedron(i);
        Vertex center = polyhedron->center();
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
        transform->setTranslation(QVector3D(-center.x, -center.y, -center.z));
        //transform->setTranslation(-center(polyhedron));
        /*
        for(int j = 0; j < polyhedron->size(); ++j)
        {
            QEntity* entity = new QEntity(_rootEntity);
            QGeometryRenderer* geometryRenderer = createGeomentyRenderer(polyhedron->polygon(j));
            entity->addComponent(material);
            entity->addComponent(transform);
            entity->addComponent(geometryRenderer);
            _entities.push_back(entity);
        }
        */

        /// Создаем сущность
        QEntity* entity = new QEntity(_rootEntity);

        appendGeomentyRenderer(entity, tree->polyhedron(i));
        entity->addComponent(material);
        entity->addComponent(transform);
        _entities.push_back(entity);


    }
}

QWidget* ModelViewerWidget_Qt3D::container()
{
    return _container;
}

void ModelViewerWidget_Qt3D::clearEntities()
{
    for(QEntity* entity : _entities)
        entity->deleteLater();
    _entities.clear();
}

void ModelViewerWidget_Qt3D::clearSourcesOfLight()
{
    for(QAbstractLight* light : _sourcesOfLight)
        light->deleteLater();
    _sourcesOfLight.clear();
}


///-------------------------------------------------------------
/// Отрисовка через Triangles
///-------------------------------------------------------------

QGeometry* ModelViewerWidget_Qt3D::createGeomenty(Polyhedron* polyhedron)
{
    QGeometry *result = new Qt3DRender::QGeometry(_rootEntity);
    createVertexAttribute(result, polyhedron);
    createIndexesAttribute(result, polyhedron);
    return result;
}

QGeometryRenderer* ModelViewerWidget_Qt3D::createGeomentyRenderer(Polyhedron* polyhedron)
{
    QGeometryRenderer *result = new QGeometryRenderer(_rootEntity);
    result->setGeometry(createGeomenty(polyhedron));
    result->setPrimitiveType(QGeometryRenderer::Triangles);
    return result;
}

void ModelViewerWidget_Qt3D::appendGeomentyRenderer(QEntity* entity, Polyhedron* polyhedron)
{
    QGeometryRenderer* geometryRenderer = createGeomentyRenderer(polyhedron);
    entity->addComponent(geometryRenderer);
}

void ModelViewerWidget_Qt3D::createVertexAttribute(QGeometry* geometry, Polyhedron* polyhedron)
{
    /// Создаем атрибут отображения геометрии (позиции точек в пространстве)
    Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute(geometry);
    /// Добавляем в геометрию объекта наш атрибут
    geometry->addAttribute(positionAttribute);

    int countPoints = 0;
    QByteArray bufferVertex;
    for(int i = 0; i < polyhedron->size(); ++i)
    {
        Polygon* polygon = polyhedron->polygon(i);
        int additionSize = polygon->size() * sizeof(Vertex);
        bufferVertex.reserve(bufferVertex.size() + additionSize);
        countPoints += polygon->size();
        if(polygon->isClockwise())
        {
            for(unsigned j = 0; j != polygon->size(); ++j)
            {
                Vertex tmp = polygon->vertex(j);
                bufferVertex.append((char*)&tmp, sizeof(Vertex));
            }
        }
        else
        {
            for(unsigned j = polygon->size(); j-- != 0; )
            {
                Vertex tmp = polygon->vertex(j);
                bufferVertex.append((char*)&tmp, sizeof(Vertex));
            }
        }
    }

    /// Создаем буфер, связанный с геометрией объекта
    Qt3DRender::QBuffer *pointsBuf = new Qt3DRender::QBuffer(geometry);
    /// Передаем в него точки
    pointsBuf->setData(bufferVertex);

    /// Указываем название атрибута (по умолчанию для позиций точек)
    positionAttribute->setName(QAttribute::defaultPositionAttributeName());
    /// Указываем, какой класс является базовым для Vertex
    positionAttribute->setVertexBaseType(QAttribute::Float);
    /// Указываем число измерений в Vertex
    positionAttribute->setVertexSize(sizeof(Vertex));
    /// Указываем, что атрибут отвечает за позиции точек в пространстве
    positionAttribute->setAttributeType(QAttribute::VertexAttribute);
    /// Передаем точки
    positionAttribute->setBuffer(pointsBuf);
    /// Указываем, какой интервал между точками
    positionAttribute->setByteStride(sizeof(Vertex));
    /// Указываем количество точек
    positionAttribute->setCount(countPoints);
}

void ModelViewerWidget_Qt3D::createIndexesAttribute(QGeometry* geometry, Polyhedron* polyhedron)
{
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
        for(unsigned j = 2; j != polygon->size(); ++j)
        {
            triangle.second = triangle.third;
            triangle.third = triangle.second + 1;
            bufferIndexes.append((char*)&triangle, sizeof(triangle));
        }
        index += polygon->size();
        countPoints +=  (polygon->size() - 2) * 3;
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
    indexAttribute->setCount(countPoints);
    /// Добавляем в геометрию объекта наш атрибут
    geometry->addAttribute(indexAttribute);
}



///-------------------------------------------------------------
/// Отрисовка через TriangleFan
///-------------------------------------------------------------
/*
void ModelViewerWidget_Qt3D::appendGeomentyRenderer(QEntity* entity, Polyhedron* polyhedron)
{
    for(int i = 0; i < polyhedron->size(); ++i)
    {
        QGeometryRenderer* geometryRenderer = createGeomentyRenderer(polyhedron->polygon(i));
        entity->addComponent(geometryRenderer);
    }
}

QGeometry* ModelViewerWidget_Qt3D::createGeomenty(Polygon* polygon)
{
    QGeometry *result = new Qt3DRender::QGeometry(_rootEntity);
    appendPositionAttribute(result, polygon);
    appendIndexesAttribute(result, polygon);
    return result;
}

QGeometryRenderer* ModelViewerWidget_Qt3D::createGeomentyRenderer(Polygon* polygon)
{
    QGeometryRenderer *result = new QGeometryRenderer(_rootEntity);
    result->setGeometry(createGeomenty(polygon));
    result->setPrimitiveType(QGeometryRenderer::TriangleFan);
    return result;
}


/// TODO добавить нормали
void ModelViewerWidget_Qt3D::appendPositionAttribute(QGeometry* geometry, Polygon* polygon)
{
    QByteArray bufferVertex;
    bufferVertex.reserve(polygon->size() * sizeof(Vertex));
    if(polygon->isClockwise())
    {
        for(unsigned i = 0; i != polygon->size(); ++i)
        {
            Vertex tmp = polygon->vertex(i);
            bufferVertex.append((char*)&tmp, sizeof(Vertex));
        }
    }
    else
    {
        for(unsigned i = polygon->size(); i-- != 0; )
        {
            Vertex tmp = polygon->vertex(i);
            bufferVertex.append((char*)&tmp, sizeof(Vertex));
        }
    }
    /// Создаем атрибут отображения геометрии (позиции точек в пространстве)
    QAttribute *positionAttribute = new QAttribute(geometry);
    /// Добавляем в геометрию объекта наш атрибут
    geometry->addAttribute(positionAttribute);
    /// Создаем буфер, связанный с геометрией объекта
    Qt3DRender::QBuffer *pointsBuf = new Qt3DRender::QBuffer(geometry);
    /// Передаем в него точки
    pointsBuf->setData(bufferVertex);
    /// Указываем название атрибута (по умолчанию для позиций точек)
    positionAttribute->setName(QAttribute::defaultPositionAttributeName());
    /// Указываем, какой класс является базовым для Vertex
    positionAttribute->setVertexBaseType(QAttribute::Float);
    /// Указываем число измерений в Vertex
    positionAttribute->setVertexSize(sizeof(Vertex));
    /// Указываем, что атрибут отвечает за позиции точек в пространстве
    positionAttribute->setAttributeType(QAttribute::VertexAttribute);
    /// Передаем точки
    positionAttribute->setBuffer(pointsBuf);
    /// Указываем, какой интервал между точками
    positionAttribute->setByteStride(sizeof(Vertex));
    /// Указываем количество точек
    positionAttribute->setCount(polygon->size());
}

void ModelViewerWidget_Qt3D::appendIndexesAttribute(QGeometry* geometry, Polygon* polygon)
{
    int countPoints = (polygon->size() - 2) * 2 + 1;
    /// Буфер, хранящий номера точек
    QByteArray bufferIndexes;
    unsigned i = 0;
    bufferIndexes.reserve(countPoints * sizeof(unsigned short));
    bufferIndexes.append((char*)&i, sizeof(unsigned short));
    for(i = 2; i != polygon->size(); ++i)
    {
        unsigned tmp = i - 1;
        bufferIndexes.append((char*)&tmp, sizeof(unsigned short));
        bufferIndexes.append((char*)&i, sizeof(unsigned short));
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
    indexAttribute->setCount(countPoints);
    /// Добавляем в геометрию объекта наш атрибут
    geometry->addAttribute(indexAttribute);
}

*/

}
