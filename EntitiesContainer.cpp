#include "EntitiesContainer.h"

namespace bstu {

EntitiesContainer::EntitiesContainer(QObject *parrent) :
    AbstractSet<PolyhedronExtension>(parrent),
    AbstractSet<QEntity>(parrent) {

}

/// Виртуальные методы множества многогранников
bool EntitiesContainer::has(PolyhedronExtension* polyhedron) {
    return container.find(polyhedron) != container.end();
}

bool EntitiesContainer::_setAppend(PolyhedronExtension* polyhedron) {
    EntityPair pair;
    pair.key = nullptr;
    pair.value = nullptr;
    auto iterator = container.find(polyhedron);
    if(iterator != container.end()) {
        pair.key = polyhedron;
        pair.value = iterator.value();
        if(pair.value != nullptr)
            emit AbstractSet<QEntity>::removed(iterator.value());
    }
    container.insert(polyhedron, nullptr);
    if(pair.key == nullptr) {
        emit AbstractEntityMap::appended(pair);
        return true;
    }
    else {
        if(pair.value != nullptr) {
            emit AbstractEntityMap::changed(pair, nullptr);
        }
        return false;
    }
}

bool EntitiesContainer::_setRemove(PolyhedronExtension* polyhedron) {
    auto iterator = container.find(polyhedron);
    if(iterator != container.end()) {
        EntityPair pair;
        pair.key = iterator.key();
        pair.value = iterator.value();
        container.remove(polyhedron);
        emit AbstractSet<QEntity>::removed(pair.value);
        emit AbstractEntityMap::removed(pair);
        return true;
    }
    return false;
}

/// Виртуальные методы множества многогранников
bool EntitiesContainer::has(QEntity* entity) {
    auto key = container.key(entity, nullptr);
    return key != nullptr;
}

bool EntitiesContainer::_setAppend(QEntity*) {
    throw NotImplementedException();
}

bool EntitiesContainer::_setRemove(QEntity* entity) {
    auto key = container.key(entity);
    auto iterator = container.find(key);
    if(iterator != container.end()) {
        EntityPair pair;
        pair.key = iterator.key();
        pair.value = iterator.value();
        container.remove(key);
        emit AbstractSet<PolyhedronExtension>::removed(pair.key);
        emit AbstractEntityMap::removed(pair);
        return true;
    }
    return false;
}

/// Виртуальные методы словаря
bool EntitiesContainer::hasValue(QEntity* value) const {
    QList<QEntity*> list = container.values();
    return std::binary_search(list.begin(), list.end(), value);
}

bool EntitiesContainer::hasKey(PolyhedronExtension* key) const {
    return container.find(key) != container.end();
}

QEntity* EntitiesContainer::get(PolyhedronExtension* key) const {
    return container.value(key);
}

class QHashEnumerator : public AbstractEnumerator<EntityPair> {
private:
    EntityPair pair;
    QHash<PolyhedronExtension*, QEntity*>::ConstIterator curr, end;
public:
    QHashEnumerator(const QHash<PolyhedronExtension*, QEntity*> &hash) {
        curr = hash.begin();
        end = hash.end();
        pair.key = nullptr;
        pair.value = nullptr;
    }

    virtual EntityPair current() const {
        return pair;
    }

    virtual bool moveNext() {
        if(curr == end) return false;
        pair.key = curr.key();
        pair.value = curr.value();
        ++curr;
        return true;
    }
};

Enumerator<EntityPair> EntitiesContainer::getEnumerator() const {
    return Enumerator<EntityPair>(new QHashEnumerator(container));
}

EntityPair EntitiesContainer::parrent(PolyhedronExtension* key) const {
    EntityPair pair;
    if(key) key = key->parrent();
    while(key) {
        auto iterator = container.find(key);
        if(iterator != container.end()) {
            pair.key = iterator.key();
            pair.value = iterator.value();
            return pair;
        }
        key = key->parrent();
    }
    pair.key = nullptr;
    pair.value = nullptr;
    return pair;
}

class ChildsEnumerator : public AbstractEnumerator<EntityPair> {
private:
    EntityPair pair;
    QStack<PolyhedronExtension*> stack;
    const QHash<PolyhedronExtension*, QEntity*> &hash;
public:
    ChildsEnumerator(const QHash<PolyhedronExtension*, QEntity*> &_hash, PolyhedronExtension* key) : hash(_hash) {
        pair.key = nullptr;
        pair.value = nullptr;
        if(key->child2())
            stack.append(key->child2());
        if(key->child1())
            stack.append(key->child1());
    }

    virtual EntityPair current() const {
        return pair;
    }

    virtual bool moveNext() {
        while(!stack.empty()) {
            PolyhedronExtension* polyhedron = stack.pop();
            auto iterator = hash.find(polyhedron);
            if(iterator == hash.end()) {
                if(polyhedron->child2())
                    stack.append(polyhedron->child2());
                if(polyhedron->child1())
                    stack.append(polyhedron->child1());
            } else {
                pair.key = iterator.key();
                pair.value = iterator.value();
                return true;
            }
        }
        pair.key = nullptr;
        pair.value = nullptr;
        return false;
    }
};

class ChildsEnumerable : public AbstractEnumerable<EntityPair> {
private:
    PolyhedronExtension* key;
    const QHash<PolyhedronExtension*, QEntity*> &hash;
public:
    ChildsEnumerable(const QHash<PolyhedronExtension*, QEntity*> &_hash, PolyhedronExtension* _key) : key(_key), hash(_hash) {

    }

    virtual Enumerator<EntityPair> getEnumerator() const {
        return Enumerator<EntityPair>(new ChildsEnumerator(hash, key));
    }
};

Enumerable<EntityPair> EntitiesContainer::childs(PolyhedronExtension* key) const {
    return Enumerable<EntityPair>(new ChildsEnumerable(container, key));
}

bool EntitiesContainer::_mapAppend(PolyhedronExtension* key, QEntity* value) {
    EntityPair pair;
    pair.key = nullptr;
    pair.value = nullptr;
    auto iterator = container.find(key);
    if(iterator != container.end()) {
        pair.key = iterator.key();
        pair.value = iterator.value();
        if(pair.value != value)
            emit AbstractSet<QEntity>::removed(pair.value);
    }
    container.insert(key, value);
    if(pair.key == nullptr) {
        emit AbstractSet<PolyhedronExtension>::appended(pair.key);
        emit AbstractSet<QEntity>::appended(pair.value);
        return true;
    }
    else {
        if(pair.value != value) {
            emit AbstractSet<QEntity>::appended(pair.value);
        }
        return false;
    }
}

bool EntitiesContainer::_mapRemove(PolyhedronExtension* key) {
    auto iterator = container.find(key);
    if(iterator != container.end()) {
        EntityPair pair;
        pair.key = iterator.key();
        pair.value = iterator.value();
        container.remove(key);
        emit AbstractSet<PolyhedronExtension>::removed(pair.key);
        emit AbstractSet<QEntity>::removed(pair.value);
        return true;
    }
    return false;
}

}


