#ifndef ENTITIESCONTAINER_H
#define ENTITIESCONTAINER_H

#include "collections/AbstractSet.h"
#include "collections/AbstractEntityMap.hpp"
#include "exceptions/NotImplementedException.hpp"

#include <QHash>

namespace bstu {


class EntitiesContainer : public AbstractSet<PolyhedronExtension>,
                          public AbstractSet<QEntity>,
                          public AbstractEntityMap
{
public:
    explicit EntitiesContainer(QObject* parrent);
private:
    /// Виртуальные методы множества многогранников
    bool has(PolyhedronExtension* polyhedron) override;
    bool _setAppend(PolyhedronExtension* polyhedron) override;
    bool _setRemove(PolyhedronExtension* polyhedron) override;
    /// Виртуальные методы множества сущностей
    bool has(QEntity* entity) override;
    bool _setAppend(QEntity* entity) override;
    bool _setRemove(QEntity* entity) override;
    /// Виртуальные методы словаря
    bool hasValue(QEntity* value) const override;
    bool hasKey(PolyhedronExtension* key) const override;
    QEntity* get(PolyhedronExtension* key) const override;
    Enumerator<EntityPair> getEnumerator() const override;
    EntityPair parrent(PolyhedronExtension* key) const override;
    Enumerable<EntityPair> childs(PolyhedronExtension* key) const override;
    bool _mapAppend(PolyhedronExtension* key, QEntity* value) override;
    bool _mapRemove(PolyhedronExtension* key) override;
    /// Контейнер всех данных
    QHash<PolyhedronExtension*, QEntity*> container;
};

}


#endif // ENTITIESCONTAINER_H
