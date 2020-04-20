#ifndef ABSTRACTMAP_H
#define ABSTRACTMAP_H

#include "collections/Enumerable.hpp"
#include "extensions/PolyhedronExtension.h"

#include "exceptions/NotImplementedException.hpp"

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
    explicit AbstractEntityMap(QObject *parent = nullptr);
    virtual int count() const {
        throw NotImplementedException();
    }
    virtual bool hasValue(QEntity* ) const {
        throw NotImplementedException();
    }
    virtual bool hasKey(PolyhedronExtension* ) const {
        throw NotImplementedException();
    }
    virtual QEntity* get(PolyhedronExtension* ) const {
        throw NotImplementedException();
    }
    virtual EntityPair parent(PolyhedronExtension* ) const {
        throw NotImplementedException();
    }
    virtual Enumerable<EntityPair> childs(PolyhedronExtension* ) const {
        throw NotImplementedException();
    }
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
    virtual bool _mapAppend(PolyhedronExtension* key, QEntity* newValue, QEntity*& oldValue) {
        throw NotImplementedException();
    }
    virtual bool _mapChange(PolyhedronExtension* key, QEntity* newValue, QEntity*& oldValue) {
        throw NotImplementedException();
    }
    virtual bool _mapRemove(PolyhedronExtension* key) {
        throw NotImplementedException();
    }
};
}
#endif // ABSTRACTMAP_H
