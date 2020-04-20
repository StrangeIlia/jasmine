#include "Activator.h"

namespace bstu {
Activator::Activator(AbstractEntityMap* map) : QObject(map)
{
    this->map = map;
    connect(map, SIGNAL(append(EntityPair)), this, SLOT(append(EntityPair)));
    connect(map, SIGNAL(changed(EntityPair, QEntity*)), this, SLOT(append(EntityPair, QEntity*)));
    connect(map, SIGNAL(remove(EntityPair)), this, SLOT(remove(EntityPair)));
}

void Activator::append(EntityPair pair) {
    if(pair.key == nullptr || pair.value == nullptr) return;
    Enumerator<EntityPair> parent = map->childs(pair.key)->getEnumerator();
    if(!parent->moveNext()) return;
    Enumerator<EntityPair> childs = map->childs(pair.key)->getEnumerator();
    while(childs->moveNext()) {
        childs->current().value->setEnabled(false);
    }
}

void Activator::append(EntityPair pair, QEntity* newEntity) {
    if(newEntity == nullptr || pair.key == nullptr) return;
    if(pair.value != nullptr) remove(pair);
    pair.value = newEntity;
    append(pair);
}

void Activator::remove(EntityPair pair) {
    if(pair.key == nullptr || pair.value == nullptr) return;
    Enumerator<EntityPair> parent = map->childs(pair.key)->getEnumerator();
    if(!parent->moveNext()) return;
    Enumerator<EntityPair> childs = map->childs(pair.key)->getEnumerator();
    while(childs->moveNext()) {
        childs->current().value->setEnabled(true);
    }
}


}

