#ifndef TRANSFORMSCONTAINER_H
#define TRANSFORMSCONTAINER_H

#include "AbstractTransformSet.h"
#include "Enumerable.hpp"

#include <Qt3DCore>

#include <QSet>

namespace bstu {
class TransformsSet : public AbstractTransformSet,
                      public AbstractEnumerable<Qt3DCore::QTransform*>
{
public:
    explicit TransformsSet(QObject *parent = nullptr);

    bool has(Qt3DCore::QTransform* key) override;
    int count() const override;

    Enumerator<Qt3DCore::QTransform*> getEnumerator() const override;
private:
    bool _setAppend(Qt3DCore::QTransform* key) override;
    bool _setRemove(Qt3DCore::QTransform* key) override;

    QSet<Qt3DCore::QTransform*> set;
};
}


#endif // TRANSFORMSCONTAINER_H
