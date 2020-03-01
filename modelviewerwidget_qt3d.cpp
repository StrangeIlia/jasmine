#include "modelviewerwidget_qt3d.h"

namespace bstu {

using namespace Qt3DCore;
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


    /// Говорим, что камера будет орбитальной (QOrbitCameraController)
    MyOrbitCameraController* orbitCameraController = new MyOrbitCameraController(_rootEntity);
    orbitCameraController->setCamera(cameraEntity);
    /// Устанавливаем предел на приближение (на отдаление предела нет)
    orbitCameraController->setZoomInLimit(4.0f);
    _cameraController = orbitCameraController;

    /// Настраиваем освещение
    Qt3DCore::QEntity *lightEntity = new QEntity(_rootEntity);
    Qt3DRender::QPointLight *light = new QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1);
    lightEntity->addComponent(light);
    /// Задаем позицию источника света как отдельное свойство
    /// Прикрепляем позицию источника света к позиции камеры
    Qt3DCore::QTransform *lightTransform = cameraEntity->componentsOfType<Qt3DCore::QTransform>().first();
    lightEntity->addComponent(lightTransform);

    _selectedObjectMaterial = new QPhongAlphaMaterial(_rootEntity);
    _selectedObjectMaterial->setShininess(0.8f);
    _selectedObjectMaterial->setDiffuse(Qt::red);

    _screenRayCaster = new QScreenRayCaster(_rootEntity);
    _screenRayCaster->setRunMode(Qt3DRender::QAbstractRayCaster::SingleShot);
    _rootEntity->addComponent(_screenRayCaster);

    QObject::connect(_screenRayCaster, SIGNAL(hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits)),
                     this, SLOT(hitsEvent(const Qt3DRender::QAbstractRayCaster::Hits)));

    _sourcesOfLight.push_back(light);
}

ModelViewerWidget_Qt3D::~ModelViewerWidget_Qt3D()
{
    clearEntities();
    clearSourcesOfLight();
    _cameraController->deleteLater();
    _rootEntity->deleteLater();
    _container->deleteLater();
    _selectedObjectMaterial->deleteLater();
}

inline Vertex centerPolyhedron(Polyhedron* polyhedron)
{
    Vertex max, min;
    max.x = max.y = max.z = std::numeric_limits<float>::min();
    min.x = min.y = min.z = std::numeric_limits<float>::max();
    for(int i = 0; i != polyhedron->size(); ++i)
    {
        Polygon* polygon = polyhedron->polygon(i);
        for(unsigned j = 0; j != polygon->size(); ++j)
        {
            Vertex tmp = polygon->vertex(j);
            max.x = std::max(max.x, tmp.x);
            max.y = std::max(max.y, tmp.y);
            max.z = std::max(max.z, tmp.z);
            min.x = std::min(min.x, tmp.x);
            min.y = std::min(min.y, tmp.y);
            min.z = std::min(min.z, tmp.z);
        }
    }
    max.x += min.x; max.x /= 2;
    max.y += min.y; max.y /= 2;
    max.z += min.z; max.z /= 2;
    return max;
}

void ModelViewerWidget_Qt3D::setTree(Tree* tree, bool entitiesEnabled)
{
    clearEntities();
    QPhongAlphaMaterial *material = new QPhongAlphaMaterial(_rootEntity);
    /// Указываем цвет материала
    material->setAmbient(QColor::fromRgbF(50.0 / 255.0, 130.0 / 255.0, 200.0 / 255.0, 1));
    material->setShininess(0.8f);
    material->setAlpha(1.0f);
    for (unsigned i = 0; i != tree->polyhedronsCount(); i++)
    {
        Polyhedron* polyhedron = tree->polyhedron(i);
        Vertex center = centerPolyhedron(polyhedron);/*polyhedron->center();*/
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
        transform->setTranslation(QVector3D(-center.x, -center.y, -center.z));
        /// Создаем сущность
        QEntity* entity = new QEntity(_rootEntity);
        entity->setEnabled(entitiesEnabled);
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

const QVector<QEntity*>& ModelViewerWidget_Qt3D::entities()
{
    return _entities;
}

// Цвет выделяемой сущности
QPhongAlphaMaterial* ModelViewerWidget_Qt3D::selectedObjectMaterial()
{
    return _selectedObjectMaterial;
}

void ModelViewerWidget_Qt3D::mousePressEvent(QMouseEvent* event)
{
    if(event->button() & Qt::MouseButton::LeftButton)
        _screenRayCaster->trigger(event->pos());
}

void ModelViewerWidget_Qt3D::hitsEvent(const Qt3DRender::QAbstractRayCaster::Hits hits)
{
    QEntity* entity = nullptr;

    float min = std::numeric_limits<float>::max();
    for(const QRayCasterHit& hit : hits)
    {
        float dist = hit.distance();
        if(min > dist)
        {
            min = dist;
            entity = hit.entity();
        }
    }

    if(_selectedEntity != entity)
    {
        if(_selectedEntity != nullptr)
        {
            _selectedEntity->removeComponent(_selectedObjectMaterial);
            _selectedEntity->addComponent(_oldSelectedObjectMaterial);
        }
        if(entity != nullptr)
        {
            _oldSelectedObjectMaterial = entity->componentsOfType<QPhongAlphaMaterial>().first();
            entity->removeComponent(_oldSelectedObjectMaterial);
            entity->addComponent(_selectedObjectMaterial);
            _selectedEntity = entity;
        }
        else
        {
            _selectedEntity = nullptr;
            _oldSelectedObjectMaterial = nullptr;
        }
    }
    else if(_selectedEntity != nullptr)
    {
        QPhongAlphaMaterial* material = entity->componentsOfType<QPhongAlphaMaterial>().first();;
        if(material == _oldSelectedObjectMaterial)
        {
            entity->removeComponent(_oldSelectedObjectMaterial);
            entity->addComponent(_selectedObjectMaterial);
        }
        else
        {
            _selectedEntity->removeComponent(_selectedObjectMaterial);
            _selectedEntity->addComponent(_oldSelectedObjectMaterial);
        }
    }
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
            for(unsigned j = 0; j != polygon->size(); ++j)
            {
                Vertex tmp = polygon->vertex(j);
                bufferVertex.append((char*)&tmp, sizeof(Vertex));
                bufferVertex.append((char*)&normal, sizeof(Vertex));
            }
        }
        else
        {
            for(unsigned j = polygon->size(); j-- != 0; )
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

}
