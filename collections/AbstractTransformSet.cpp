#include "AbstractTransformSet.h"

namespace bstu {
AbstractTransformSet::AbstractTransformSet(QObject* parrent) : QObject(parrent) {

}

bool AbstractTransformSet::append(Qt3DCore::QTransform* key) {
    if(key == nullptr) return false;
    if(_setAppend(key)) {
        emit appended(key);
        return true;
    }
    return false;
}

bool AbstractTransformSet::remove(Qt3DCore::QTransform* key) {
    if(key == nullptr) return false;
    if(_setRemove(key)) {
        emit removed(key);
        return true;
    }
    return false;
}
}
