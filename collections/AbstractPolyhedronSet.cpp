#include "AbstractPolyhedronSet.h"

namespace bstu {
AbstractPolyhedronSet::AbstractPolyhedronSet(QObject* parrent) : QObject(parrent) {

}

bool AbstractPolyhedronSet::append(PolyhedronExtension* key) {
    if(key == nullptr) return false;
    if(_setAppend(key)) {
        emit appended(key);
        return true;
    }
    return false;
}

bool AbstractPolyhedronSet::remove(PolyhedronExtension* key) {
    if(key == nullptr) return false;
    if(_setRemove(key)) {
        emit removed(key);
        return true;
    }
    return false;
}
}
