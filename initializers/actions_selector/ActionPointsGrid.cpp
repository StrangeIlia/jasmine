#include "ActionPointsGrid.h"

namespace bstu {
ActionPointsGrid::ActionPointsGrid(QMaterial* pointColor) : color(pointColor)
{
    color->setShareable(true);
}

void ActionPointsGrid::entitySelected(QEntity* entity) {
    QVector<SmartComponent*> vector = entity->componentsOfType<SmartComponent>();
    if(vector.size() == 0) return;

    SmartComponent* component = vector.first();
    Qt3DCore::QTransform* transform = entity->componentsOfType<Qt3DCore::QTransform>().first();
    transform->setShareable(true);

    if(processedEntity != nullptr) processedEntity->deleteLater();

    processedEntity = new QEntity(entity);

    color->moveToThread(entity->thread());
    processedEntity->moveToThread(entity->thread());
    PointsGeometryFactory* factory = new PointsGeometryFactory();
    processedEntity->addComponent(factory->create(component->polyhedron()->data()));
    delete factory;
    processedEntity->addComponent(color);

}

void ActionPointsGrid::entityUnselected(QEntity* entity) {
    if(processedEntity == nullptr) return;
    if(processedEntity->components().contains(color)) {
        color->removedFromEntity(processedEntity);
    }
    processedEntity->deleteLater();
    processedEntity = nullptr;
}


}
