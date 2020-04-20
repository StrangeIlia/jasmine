#ifndef ABSTRACTITERATOR_H
#define ABSTRACTITERATOR_H

#include <QSharedPointer>

namespace bstu {
template<class T> class AbstractEnumerator {
public:
    AbstractEnumerator() {}
    virtual ~AbstractEnumerator() {}
    virtual T current() const = 0;
    virtual bool moveNext() = 0;
};

template<class T> class Enumerator : public QSharedPointer<AbstractEnumerator<T>> {
public:
    Enumerator(AbstractEnumerator<T>* ptr) : QSharedPointer<AbstractEnumerator<T>>(ptr) {

    }
    Enumerator(Enumerator&& enumerator) : QSharedPointer<AbstractEnumerator<T>>(enumerator) {

    }
    Enumerator(const Enumerator& enumerator) : QSharedPointer<AbstractEnumerator<T>>(enumerator) {

    }
};

}

#endif // ABSTRACTITERATOR_H
