#ifndef CLEANER_H
#define CLEANER_H

#include "collections/AbstractEntityMap.hpp"

namespace bstu {
class Cleaner : public QObject {
    Q_OBJECT
public:
    Cleaner(AbstractEntityMap* map) : QObject(map) {
        connect(map, SIGNAL(removed(EntityPair)), this, SLOT(removeEmtity(EntityPair)));
    }
private slots:
    void removeEmtity(EntityPair pair) {
        pair.value->deleteLater();
    }
};
}

#endif // CLEANER_H
