#ifndef ENTITIESCONTAINER_H
#define ENTITIESCONTAINER_H

#include "collections/AbstractEntitySet.h"
#include "collections/AbstractEntityMap.hpp"
#include "collections/AbstractPolyhedronSet.h"
#include "exceptions/NotImplementedException.hpp"

#include <QHash>
#include <QMutex>

#define PARALLEL_CONTAINER_HANDLING 0

namespace bstu {


class EntitiesContainer : public AbstractPolyhedronSet,
                          public AbstractEntitySet,
                          public AbstractEntityMap
{
public:
    explicit EntitiesContainer(QObject* parrent = nullptr);
private:
    int count() const override;

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
    bool _mapAppend(PolyhedronExtension* key, QEntity* value, QEntity*& oldValue) override;
    bool _mapChange(PolyhedronExtension* key, QEntity* value, QEntity*& oldValue) override;
    bool _mapRemove(PolyhedronExtension* key) override;
#if PARALLEL_CONTAINER_HANDLING
    /// Для параллельной обработки
    QMutex mutex;
#endif
    /// Контейнер всех данных
    QHash<PolyhedronExtension*, QEntity*> container;
};

}


#endif // ENTITIESCONTAINER_H
