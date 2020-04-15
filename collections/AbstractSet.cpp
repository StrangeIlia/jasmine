#include "AbstractSet.h"

namespace bstu {
template<class T> AbstractSet<T>::AbstractSet(QObject* parrent) : QObject(parrent) {

}

template<class T> bool AbstractSet<T>::append(T* key) {
    if(key == nullptr) return false;
    if(_setAppend(key)) {
        emit appended(key);
        return true;
    }
    return false;
}

template<class T> bool AbstractSet<T>::remove(T* key) {
    if(key == nullptr) return false;
    if(_setRemove(key)) {
        emit removed(key);
        return true;
    }
    return false;
}
}
