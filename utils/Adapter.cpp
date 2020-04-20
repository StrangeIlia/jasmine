#include "Adapter.h"

namespace bstu {
Adapter::Adapter(View3D* view, AbstractEntitySet* set) : QObject(view) {
    this->set = set;
    this->view = view;
    connect(set, SIGNAL(appended(QEntity*)), SLOT(addEntity(QEntity*)));
    connect(set, SIGNAL(removed(QEntity*)), SLOT(removeEntity(QEntity*)));
}

void Adapter::addEntity(QEntity* entity) {
    if(entity != nullptr) {
        entity->moveToThread(view->rootEntity()->thread());
        entity->setParent(view->rootEntity());
    }
}

void Adapter::removeEntity(QEntity* entity) {
    if(entity != nullptr)
        entity->setParent((QNode*)nullptr);
}

}
