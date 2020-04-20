#ifndef ACTIVATOR_H
#define ACTIVATOR_H

#include <QObject>

#include "collections/AbstractEntityMap.hpp"

namespace bstu {
class Activator : public QObject
{
    Q_OBJECT
public:
    explicit Activator(AbstractEntityMap* map);
private slots:
    void append(EntityPair pair);
    void append(EntityPair pair, QEntity* newEntity);
    void remove(EntityPair pair);
private:
    AbstractEntityMap* map;
};
}


#endif // ACTIVATOR_H
