#ifndef ENUMERABLE_HPP
#define ENUMERABLE_HPP

#include "enumerator/Enumerator.hpp"

namespace bstu {
template<class T>
class AbstractEnumerable {
public:
    virtual Enumerator<T> getEnumerator() const = 0;
};

template<class T> class Enumerable : public QSharedPointer<AbstractEnumerable<T>> {
public:
    Enumerable(AbstractEnumerable<T>* ptr) : QSharedPointer<AbstractEnumerator<T>>(ptr) {

    }
    Enumerable(Enumerable&& enumerable) : QSharedPointer<AbstractEnumerator<T>>(enumerable) {

    }
    Enumerable(const Enumerable& enumerable) : QSharedPointer<AbstractEnumerator<T>>(enumerable) {

    }
};
}

#endif // ENUMERABLE_HPP
