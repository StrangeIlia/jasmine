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
    Enumerable(AbstractEnumerable<T>* ptr) : QSharedPointer<AbstractEnumerable<T>>(ptr) {

    }
    Enumerable(Enumerable&& enumerable) : QSharedPointer<AbstractEnumerable<T>>(enumerable) {

    }
    Enumerable(const Enumerable& enumerable) : QSharedPointer<AbstractEnumerable<T>>(enumerable) {

    }
};
}

#endif // ENUMERABLE_HPP
