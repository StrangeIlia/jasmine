#include "EntitiesContainer.h"

namespace bstu {

EntitiesContainer::EntitiesContainer(QObject *parrent) :
    AbstractSet<PolyhedronExtension>(parrent),
    AbstractSet<QEntity>(parrent) {

}

int EntitiesContainer::count() const {
    return container.count();
}

/// Виртуальные методы множества многогранников
bool EntitiesContainer::has(PolyhedronExtension* polyhedron) {
    return container.find(polyhedron) != container.end();
}

bool EntitiesContainer::_setAppend(PolyhedronExtension* polyhedron) {
#if PARALLEL_CONTAINER_HANDLING
    mutex.lock();
#endif
    EntityPair pair;
    pair.key = nullptr;
    pair.value = nullptr;
    auto iterator = container.find(polyhedron);
    if(iterator != container.end()) {
        pair.key = polyhedron;
        pair.value = iterator.value();
        if(pair.value != nullptr)
            emit AbstractSet<QEntity>::removed(iterator.value());
        else {
#if PARALLEL_CONTAINER_HANDLING
            mutex.unlock();
#endif
            return false;
        }
    }
    container.insert(polyhedron, nullptr);
    if(pair.key == nullptr) {
        emit AbstractEntityMap::appended(pair);
#if PARALLEL_CONTAINER_HANDLING
        mutex.unlock();
#endif
        return true;
    }
    else {
        if(pair.value != nullptr) {
            emit AbstractEntityMap::changed(pair, nullptr);
        }
#if PARALLEL_CONTAINER_HANDLING
        mutex.unlock();
#endif
        return false;
    }
}

bool EntitiesContainer::_setRemove(PolyhedronExtension* polyhedron) {
#if PARALLEL_CONTAINER_HANDLING
    mutex.lock();
#endif
    auto iterator = container.find(polyhedron);
    if(iterator != container.end()) {
        EntityPair pair;
        pair.key = iterator.key();
        pair.value = iterator.value();
        container.remove(polyhedron);
        emit AbstractSet<QEntity>::removed(pair.value);
        emit AbstractEntityMap::removed(pair);
#if PARALLEL_CONTAINER_HANDLING
        mutex.unlock();
#endif
        return true;
    }
#if PARALLEL_CONTAINER_HANDLING
    mutex.unlock();
#endif
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
#if PARALLEL_CONTAINER_HANDLING
    mutex.lock();
#endif
    auto key = container.key(entity);
    auto iterator = container.find(key);
    if(iterator != container.end()) {
        EntityPair pair;
        pair.key = iterator.key();
        pair.value = iterator.value();
        container.remove(key);
        emit AbstractSet<PolyhedronExtension>::removed(pair.key);
        emit AbstractEntityMap::removed(pair);
#if PARALLEL_CONTAINER_HANDLING
        mutex.unlock();
#endif
        return true;
    }
#if PARALLEL_CONTAINER_HANDLING
    mutex.unlock();
#endif
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

bool EntitiesContainer::_mapAppend(PolyhedronExtension* key, QEntity* value, QEntity*& oldValue) {
#if PARALLEL_CONTAINER_HANDLING
    mutex.lock();
#endif
    PolyhedronExtension* oldKey = nullptr;
    auto iterator = container.find(key);
    if(iterator != container.end()) {
        oldKey = iterator.key();
        oldValue = iterator.value();
        if(oldValue != value)
            emit AbstractSet<QEntity>::removed(oldValue);
        else {
#if PARALLEL_CONTAINER_HANDLING
            mutex.unlock();
#endif
            return false;
        }
    }
    container.insert(key, value);
    if(oldKey == nullptr) {
        emit AbstractSet<PolyhedronExtension>::appended(key);
        emit AbstractSet<QEntity>::appended(value);
#if PARALLEL_CONTAINER_HANDLING
        mutex.unlock();
#endif
        return true;
    }
    else {
        if(oldValue != value) {
            emit AbstractSet<QEntity>::appended(value);
        }
#if PARALLEL_CONTAINER_HANDLING
        mutex.unlock();
#endif
        return false;
    }
}

bool EntitiesContainer::_mapChange(PolyhedronExtension* key, QEntity* value, QEntity*& oldValue) {
#if PARALLEL_CONTAINER_HANDLING
    mutex.lock();
#endif
    auto iterator = container.find(key);
    if(iterator == container.end()) {
#if PARALLEL_CONTAINER_HANDLING
        mutex.unlock();
#endif
        return false;
    }
    oldValue = iterator.value();
    if(oldValue == value) {
#if PARALLEL_CONTAINER_HANDLING
        mutex.unlock();
#endif
        return false;
    }
    emit AbstractSet<QEntity>::removed(oldValue);
    container.insert(key, value);
    emit AbstractSet<QEntity>::appended(value);
    return true;
}

bool EntitiesContainer::_mapRemove(PolyhedronExtension* key) {
#if PARALLEL_CONTAINER_HANDLING
    mutex.lock();
#endif
    auto iterator = container.find(key);
    if(iterator != container.end()) {
        EntityPair pair;
        pair.key = iterator.key();
        pair.value = iterator.value();
        container.remove(key);
        emit AbstractSet<PolyhedronExtension>::removed(pair.key);
        emit AbstractSet<QEntity>::removed(pair.value);
#if PARALLEL_CONTAINER_HANDLING
        mutex.unlock();
#endif
        return true;
    }
#if PARALLEL_CONTAINER_HANDLING
    mutex.unlock();
#endif
    return false;
}

}


