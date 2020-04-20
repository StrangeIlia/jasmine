#ifndef ENTITYCONSTRUCTOR_H
#define ENTITYCONSTRUCTOR_H

#include <QObject>
#include <stdexcept>

#include "factories/AbstractGeometryFactory.hpp"
#include "factories/AbstractMaterialFactory.hpp"
#include "factories/AbstractTransformFactory.hpp"
#include "collections/AbstractTransformSet.h"
#include "collections/AbstractEntityMap.hpp"

namespace bstu {
class EntityConstructor : public QObject
{
    Q_OBJECT
public:
    explicit EntityConstructor(AbstractGeometryFactory* geometry,
                               AbstractMaterialFactory* material,
                               AbstractTransformFactory* transform,
                               AbstractEntityMap* map,
                               AbstractTransformSet* set,
                               QObject *parent = nullptr);
private slots:
    void createEntity(EntityPair pair);
private:
    AbstractEntityMap* map = nullptr;
    AbstractGeometryFactory* geometry = nullptr;
    AbstractMaterialFactory* material = nullptr;
    AbstractTransformFactory* transform = nullptr;
    AbstractTransformSet* set = nullptr;
};
}

#endif // ENTITYCONSTRUCTOR_H
