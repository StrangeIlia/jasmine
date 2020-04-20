#include "AbstractEntitySet.h"

namespace bstu {
AbstractEntitySet::AbstractEntitySet(QObject* parrent) : QObject(parrent) {

}

bool AbstractEntitySet::append(QEntity* key) {
    if(key == nullptr) return false;
    if(_setAppend(key)) {
        emit appended(key);
        return true;
    }
    return false;
}

bool AbstractEntitySet::remove(QEntity* key) {
    if(key == nullptr) return false;
    if(_setRemove(key)) {
        emit removed(key);
        return true;
    }
    return false;
}
}
