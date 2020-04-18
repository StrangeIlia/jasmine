#include "TransformsSet.h"

namespace bstu {
TransformsSet::TransformsSet(QObject *parent) : AbstractSet<Qt3DCore::QTransform>(parent) {

}

bool TransformsSet::has(Qt3DCore::QTransform* key) {
    return set.contains(key);
}

int TransformsSet::count() const {
    return set.count();
}

class SimpleEnumerator : public AbstractEnumerator<Qt3DCore::QTransform*> {
public:
    typedef QSet<Qt3DCore::QTransform*>::const_iterator Iterator;
private:
    Qt3DCore::QTransform* data = nullptr;
    Iterator begin, end;
public:
    SimpleEnumerator(Iterator begin, Iterator end) {
        this->begin = begin;
        this->end = end;
    }

    Qt3DCore::QTransform* current() const override {
        return data;
    }

    bool moveNext() override {
        if(begin == end) {
            data = nullptr;
            return false;
        } else {
            data = *begin++;
            return true;
        }
    }
};

Enumerator<Qt3DCore::QTransform*> TransformsSet::getEnumerator() const {
    return Enumerator<Qt3DCore::QTransform*>(new SimpleEnumerator(set.begin(), set.end()));
}

bool TransformsSet::_setAppend(Qt3DCore::QTransform* key) {
    if(!set.contains(key)) {
        set.insert(key);
        connect(key, SIGNAL(destroyed(QObject*)), SLOT(remove(Qt3DCore::QTransform*)));
        return true;
    }
    return false;
}

bool TransformsSet::_setRemove(Qt3DCore::QTransform* key) {
    return set.remove(key);
}

}
