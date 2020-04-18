#include "EntityConstructor.h"

namespace bstu{
EntityConstructor::EntityConstructor(AbstractGeometryFactory* geometry,
                               AbstractMaterialFactory* material,
                               AbstractTransformFactory* transform,
                               AbstractEntityMap* map,
                               AbstractSet<Qt3DCore::QTransform>* set,
                               QObject *parent) : QObject(parent) {
    this->map = map;
    this->set = set;
    this->geometry = geometry;
    this->material = material;
    this->transform = transform;
    connect(map, SIGNAL(appended(EntityPair)), SLOT(createEntity(EntityPair)));
}

void EntityConstructor::createEntity(EntityPair pair) {
    if(pair.key == nullptr) return;
    if(pair.value != nullptr) return;
    Polyhedron* polyhedron = pair.key->data();
    QEntity* entity = new QEntity();
    Qt3DCore::QTransform* transform = this->transform->create(polyhedron);
    set->append(transform);
    entity->addComponent(transform);
    entity->addComponent(geometry->create(polyhedron));
    entity->addComponent(material->create(polyhedron));
    map->change(pair.key, entity);
}

}
