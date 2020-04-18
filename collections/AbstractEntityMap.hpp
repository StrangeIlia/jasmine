#ifndef ABSTRACTMAP_H
#define ABSTRACTMAP_H

#include "collections/Enumerable.hpp"
#include "extensions/PolyhedronExtension.h"

#include <Qt3DCore>

namespace bstu {
using namespace Qt3DCore;

template<class Key, class Value> struct KeyValuePair {
    Key key;
    Value value;
};
typedef KeyValuePair<PolyhedronExtension*, QEntity*> EntityPair;

class AbstractEntityMap : public QObject, public AbstractEnumerable<EntityPair> {
    Q_OBJECT
public:
    explicit AbstractEntityMap(QObject *parrent = nullptr);
    virtual int count() const = 0;
    virtual bool hasValue(QEntity* value) const = 0;
    virtual bool hasKey(PolyhedronExtension* key) const = 0;
    virtual QEntity* get(PolyhedronExtension* key) const = 0;
    virtual EntityPair parrent(PolyhedronExtension* key) const = 0;
    virtual Enumerable<EntityPair> childs(PolyhedronExtension* key) const = 0;
public slots:
    /// Возращает истина, если такого ключа не было в коллекции
    bool append(PolyhedronExtension* key, QEntity* value);
    /// Возращает истина, если удалось изменить значение по ключу
    bool change(PolyhedronExtension* key, QEntity* value);
    /// Возвращает истина, если ключ был в коллекции
    bool remove(PolyhedronExtension* key);
signals:
    void appended(EntityPair pair);
    void removed(EntityPair pair);
    void changed(EntityPair oldPair, QEntity* newEntity);
protected:
    virtual bool _mapAppend(PolyhedronExtension* key, QEntity* newValue, QEntity*& oldValue) = 0;
    virtual bool _mapChange(PolyhedronExtension* key, QEntity* newValue, QEntity*& oldValue) = 0;
    virtual bool _mapRemove(PolyhedronExtension* key) = 0;
};
}
#endif // ABSTRACTMAP_H
