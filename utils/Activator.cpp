#include "Activator.h"

namespace bstu {
Activator::Activator(AbstractEntityMap* map) : QObject(map)
{
    this->map = map;
    connect(map, SIGNAL(appended(EntityPair)), this, SLOT(append(EntityPair)));
    connect(map, SIGNAL(changed(EntityPair, QEntity*)), this, SLOT(changed(EntityPair, QEntity*)));
    connect(map, SIGNAL(removed(EntityPair)), this, SLOT(remove(EntityPair)));
}

void Activator::append(EntityPair pair) {
    if(pair.key == nullptr || pair.value == nullptr) return;
    EntityPair parent = map->parent(pair.key);
    if(parent.key != nullptr) {
        pair.value->setEnabled(false);
        return;
    }
    Enumerator<EntityPair> childs = map->childs(pair.key)->getEnumerator();
    while(childs->moveNext()) {
        childs->current().value->setEnabled(false);
    }
}

void Activator::changed(EntityPair pair, QEntity* newEntity) {
    if(newEntity == nullptr || pair.key == nullptr) return;
    if(pair.value != nullptr) remove(pair);
    pair.value = newEntity;
    append(pair);
}

void Activator::remove(EntityPair pair) {
    if(pair.key == nullptr || pair.value == nullptr) return;
    EntityPair parent = map->parent(pair.key);
    if(parent.key != nullptr) return;
    Enumerator<EntityPair> childs = map->childs(pair.key)->getEnumerator();
    while(childs->moveNext()) {
        childs->current().value->setEnabled(true);
    }
}


}

