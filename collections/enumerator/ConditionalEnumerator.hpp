#ifndef CONDITIONALENUMERATOR_H
#define CONDITIONALENUMERATOR_H

#include <functional>
#include "Enumerator.hpp"

namespace bstu {
template<class T>
class ConditionalEnumerator : public AbstractEnumerator<T>
{
public:
    typedef std::function<bool(T*)> Condition;
private:
    Condition condition;
    Enumerator<T> baseEnumerator;
public:
    ConditionalEnumerator(Enumerator<T> baseEnumerator, Condition condition) {
        this->condition = std::move(condition);
        this->baseEnumerator = std::move(baseEnumerator);
    }

    virtual T current() const {
        return baseEnumerator->current();
    }

    virtual bool moveNext() const {
        while(baseEnumerator->moveNext()) {
            if(condition(baseEnumerator->current()))
                return true;
        }
        return false;
    }
};
}

#endif // CONDITIONALENUMERATOR_H
