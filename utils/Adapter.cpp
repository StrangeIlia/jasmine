#include "Adapter.h"

namespace bstu {
Adapter::Adapter(View3D* view, AbstractSet<QEntity>* set) : QObject(view) {
    this->set = set;
    this->view = view;
    connect(set, SIGNAL(appended(QEntity*)), SLOT(addEntity(QEntity*)));
    connect(set, SIGNAL(removed(QEntity*)), SLOT(removeEntity(QEntity*)));
}

void Adapter::addEntity(QEntity* entity) {
    entity->setParent(view->rootEntity());
}

void Adapter::removeEntity(QEntity* entity) {
    entity->setParent((QNode*)nullptr);
}

}
