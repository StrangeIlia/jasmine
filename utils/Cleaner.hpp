#ifndef CLEANER_H
#define CLEANER_H

#include "collections/AbstractEntityMap.hpp"

namespace bstu {
class Cleaner : public QObject {
    Q_OBJECT
public:
    Cleaner(AbstractEntityMap* map) : QObject(map) {
        connect(map, SIGNAL(removed(PolyhedronExtension*, QEntity*)), SLOT(removeEmtity(PolyhedronExtension*, QEntity*)));
    }
private slots:
    void removeEmtity(PolyhedronExtension*, QEntity* entity) {
        entity->deleteLater();
    }
};
}

#endif // CLEANER_H
