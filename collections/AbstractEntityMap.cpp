#include "AbstractEntityMap.hpp"

namespace bstu {
AbstractEntityMap::AbstractEntityMap(QObject* parrent) : QObject(parrent) {

}

bool AbstractEntityMap::append(PolyhedronExtension* key, QEntity* value) {
    if(key == nullptr) return false;
    if(_mapAppend(key, value)) {
        EntityPair pair;
        pair.key = key;
        pair.value = value;
        emit appended(pair);
        return true;
    }
    return false;
}

bool AbstractEntityMap::remove(PolyhedronExtension* key) {
    if(key == nullptr) return false;
    QEntity* value = get(key);
    if(value != nullptr) {
        _mapRemove(key);
        EntityPair pair;
        pair.key = key;
        pair.value = value;
        emit removed(pair);
        return true;
    }
    return false;
}

}
