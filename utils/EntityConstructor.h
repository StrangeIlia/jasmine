#ifndef ENTITYCONSTRUCTOR_H
#define ENTITYCONSTRUCTOR_H

#include <QObject>
#include <stdexcept>

#include "factories/AbstractGeometryFactory.hpp"
#include "factories/AbstractMaterialFactory.hpp"
#include "factories/AbstractTransformFactory.hpp"
#include "collections/AbstractSet.hpp"
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
                               AbstractSet<Qt3DCore::QTransform>* set,
                               QObject *parent = nullptr);
private slots:
    void createEntity(EntityPair pair);
private:
    AbstractEntityMap* map = nullptr;
    AbstractGeometryFactory* geometry = nullptr;
    AbstractMaterialFactory* material = nullptr;
    AbstractTransformFactory* transform = nullptr;
    AbstractSet<Qt3DCore::QTransform>* set = nullptr;
};
}

#endif // ENTITYCONSTRUCTOR_H
