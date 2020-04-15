#include "EntityController.h"

namespace bstu {
EntityController::EntityController(View3D *parent) : QObject(parent)
{
    View3D *view;
    try {
        view = qobject_cast<View3D*>(parent);
    } catch (std::exception) {
        throw QException();
    }
    if(parent == nullptr) throw QException();

    factory = new GeometryRendererFactory();
    material = new QPhongAlphaMaterial();
    transform = new Qt3DCore::QTransform();
    transform->setShareable(true);
    material->setShareable(true);
    material->setAmbient(QColor::fromRgbF(50.0 / 255.0, 130.0 / 255.0, 200.0 / 255.0, 1));
    material->setShininess(0.8f);
    material->setAlpha(1.0f);

    view->addComponent(material);
    view->addComponent(transform);
}

EntityController::~EntityController()
{
    delete factory;
//    material->deleteLater(); //Мы передали контроль за компонентами view
//    transform->deleteLater();
}

void EntityController::addEntity(Polyhedron* polyhedron)
{
    if(!activePolyhedrons.contains(polyhedron))
    {
        if(activePolyhedrons.size() == 0)
            minMaxPointsInit();

        View3D *view = (View3D*)parent();
        factory->setPolyhedron(polyhedron);
        QEntity* entity = new QEntity();
        entity->addComponent(material);
        entity->addComponent(transform);
        entity->addComponent(factory->createGeometryRenderer());
        centeringOnAppend(polyhedron);
        activePolyhedrons.insert(polyhedron, entity);
        view->addEntity(entity);
        if(hasParrent(polyhedron)){
            entity->setEnabled(false);
        }
        else {
            changeStateChilds(polyhedron, false);
        }
    }
}

/// TODO исправить ошибку с компонентами
void EntityController::removeEntity(Polyhedron* polyhedron)
{
    auto iter = activePolyhedrons.find(polyhedron);
    if(iter != activePolyhedrons.end())
    {
        QEntity* entity = iter.value();
        View3D *view = (View3D*)parent();
        if(iter.value()->isEnabled())
            changeStateChilds(polyhedron, true);
        activePolyhedrons.remove(polyhedron);
        view->removeEntity(entity);
        entity->deleteLater();
    }
}

bool EntityController::hasParrent(Polyhedron* polyhedron)
{
    for(auto iter = activePolyhedrons.begin(); iter != activePolyhedrons.end(); ++iter)
    {
        if(hasParrent(iter.key(), polyhedron))
            return true;
    }
    return false;
}

bool EntityController::hasParrent(Polyhedron* parent, Polyhedron* polyhedron)
{
    bool result = false;
    if(parent->isHalve())
    {
        result = result || parent->child1() == polyhedron;
        result = result || parent->child2() == polyhedron;
        if(!result)
        {
            // из - за особенности операции || hasParrent не будет вызван, если result = true
            result = result || hasParrent(parent->child1(), polyhedron);
            result = result || hasParrent(parent->child2(), polyhedron);
        }
    }

    return result;
}

void EntityController::changeStateChilds(Polyhedron* polyhedron, bool state)
{
    QQueue<Polyhedron*> polyhedrons;
    if(polyhedron->isHalve())
    {
        polyhedrons.enqueue(polyhedron->child1());
        polyhedrons.enqueue(polyhedron->child2());
    }
    while(!polyhedrons.empty())
    {
        polyhedron = polyhedrons.dequeue();
        auto iter = activePolyhedrons.find(polyhedron);
        if(iter != activePolyhedrons.end())
            iter.value()->setEnabled(state);
        else if(polyhedron->isHalve())
        {
            polyhedrons.enqueue(polyhedron->child1());
            polyhedrons.enqueue(polyhedron->child2());
        }
    }
}

void EntityController::minMaxPointsInit()
{
    min_x.polyhedron = min_y.polyhedron = min_z.polyhedron = nullptr;
    max_x.polyhedron = max_y.polyhedron = max_z.polyhedron = nullptr;
    min_x.value = min_y.value = min_z.value =  std::numeric_limits<float>::max();
    max_x.value = max_y.value = max_z.value = -std::numeric_limits<float>::max();
}

void EntityController::centeringOnAppend(Polyhedron* polyhedron)
{
    bool isChanged = false;
    for(int i = 0; i != polyhedron->size(); ++i)
    {
        Polygon* polygon = polyhedron->polygon(i);
        for(unsigned j = 0; j != polygon->size(); ++j)
        {
            Vertex v = polygon->vertex(j);
            if(v.x < min_x.value){
                isChanged = true;
                min_x.value = v.x;
                min_x.polyhedron = polyhedron;
            }
            if(v.y < min_y.value){
                isChanged = true;
                min_y.value = v.y;
                min_y.polyhedron = polyhedron;
            }
            if(v.z < min_z.value){
                isChanged = true;
                min_z.value = v.z;
                min_z.polyhedron = polyhedron;
            }
            if(v.x > max_x.value){
                max_x.value = v.x;
                max_x.polyhedron = polyhedron;
            }
            if(v.y > max_y.value){
                isChanged = true;
                max_y.value = v.y;
                max_y.polyhedron = polyhedron;
            }
            if(v.z > max_z.value){
                isChanged = true;
                max_z.value = v.z;
                max_z.polyhedron = polyhedron;
            }
        }
    }
    if(isChanged) changeTransform();
}

void EntityController::centeringOnDelete(Polyhedron* polyhedron)
{
    if(activePolyhedrons.count() == 0) return;
    bool isChanged = false;
    if(min_x.polyhedron == polyhedron)
    {
        isChanged = true;
        min_x.value = std::numeric_limits<float>::max();
        min_x.polyhedron = nullptr;
        for(auto iter = activePolyhedrons.begin(); iter != activePolyhedrons.end(); ++iter)
        {
            Polyhedron* polyhedron = iter.key();
            for(int i = 0; i != polyhedron->size(); ++i)
            {
                Polygon* polygon = polyhedron->polygon(i);
                for(unsigned j = 0; j != polygon->size(); ++j)
                {
                    Vertex v = polygon->vertex(j);
                    if(v.x < min_x.value){
                        min_x.value = v.x;
                        min_x.polyhedron = polyhedron;
                    }
                }
            }
        }
    }
    if(min_y.polyhedron == polyhedron)
    {
        isChanged = true;
        min_y.value = std::numeric_limits<float>::max();
        min_y.polyhedron = nullptr;
        for(auto iter = activePolyhedrons.begin(); iter != activePolyhedrons.end(); ++iter)
        {
            Polyhedron* polyhedron = iter.key();
            for(int i = 0; i != polyhedron->size(); ++i)
            {
                Polygon* polygon = polyhedron->polygon(i);
                for(unsigned j = 0; j != polygon->size(); ++j)
                {
                    Vertex v = polygon->vertex(j);
                    if(v.y < min_y.value){
                        min_y.value = v.y;
                        min_y.polyhedron = polyhedron;
                    }
                }
            }
        }
    }
    if(min_z.polyhedron == polyhedron)
    {
        isChanged = true;
        min_z.value = std::numeric_limits<float>::max();
        min_z.polyhedron = nullptr;
        for(auto iter = activePolyhedrons.begin(); iter != activePolyhedrons.end(); ++iter)
        {
            Polyhedron* polyhedron = iter.key();
            for(int i = 0; i != polyhedron->size(); ++i)
            {
                Polygon* polygon = polyhedron->polygon(i);
                for(unsigned j = 0; j != polygon->size(); ++j)
                {
                    Vertex v = polygon->vertex(j);
                    if(v.z < min_z.value){
                        min_z.value = v.z;
                        min_z.polyhedron = polyhedron;
                    }
                }
            }
        }
    }
    if(max_x.polyhedron == polyhedron)
    {
        isChanged = true;
        max_x.value = std::numeric_limits<float>::min();
        max_x.polyhedron = nullptr;
        for(auto iter = activePolyhedrons.begin(); iter != activePolyhedrons.end(); ++iter)
        {
            Polyhedron* polyhedron = iter.key();
            for(int i = 0; i != polyhedron->size(); ++i)
            {
                Polygon* polygon = polyhedron->polygon(i);
                for(unsigned j = 0; j != polygon->size(); ++j)
                {
                    Vertex v = polygon->vertex(j);
                    if(v.x > max_x.value){
                        max_x.value = v.x;
                        max_x.polyhedron = polyhedron;
                    }
                }
            }
        }
    }
    if(max_y.polyhedron == polyhedron)
    {
        isChanged = true;
        max_y.value = std::numeric_limits<float>::min();
        max_y.polyhedron = nullptr;
        for(auto iter = activePolyhedrons.begin(); iter != activePolyhedrons.end(); ++iter)
        {
            Polyhedron* polyhedron = iter.key();
            for(int i = 0; i != polyhedron->size(); ++i)
            {
                Polygon* polygon = polyhedron->polygon(i);
                for(unsigned j = 0; j != polygon->size(); ++j)
                {
                    Vertex v = polygon->vertex(j);
                    if(v.y > max_y.value){
                        max_y.value = v.y;
                        max_y.polyhedron = polyhedron;
                    }
                }
            }
        }
    }
    if(max_z.polyhedron == polyhedron)
    {
        isChanged = true;
        max_z.value = std::numeric_limits<float>::min();
        max_z.polyhedron = nullptr;
        for(auto iter = activePolyhedrons.begin(); iter != activePolyhedrons.end(); ++iter)
        {
            Polyhedron* polyhedron = iter.key();
            for(int i = 0; i != polyhedron->size(); ++i)
            {
                Polygon* polygon = polyhedron->polygon(i);
                for(unsigned j = 0; j != polygon->size(); ++j)
                {
                    Vertex v = polygon->vertex(j);
                    if(v.z > max_z.value){
                        max_z.value = v.z;
                        max_z.polyhedron = polyhedron;
                    }
                }
            }
        }
    }
    if(isChanged) changeTransform();
}

void EntityController::changeTransform()
{
    float aveg_x = (max_x.value + min_x.value) / 4; //WHAT? не понимаю, почему не / 2, но так оно центрует
    float aveg_y = (max_y.value + min_y.value) / 4;
    float aveg_z = (max_z.value + min_z.value) / 4;
    QMatrix4x4 matrix;
    matrix.translate(-aveg_x, -aveg_y, -aveg_z);
    transform->setMatrix(matrix);
}

}
