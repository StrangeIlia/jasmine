#ifndef SIMPLETRANSFORMFACTORY_H
#define SIMPLETRANSFORMFACTORY_H

#include "utils/factories/AbstractTransformFactory.hpp"

namespace bstu {
class SimpleTransformFactory : public AbstractTransformFactory
{
public:
    SimpleTransformFactory(QNode* root);
    virtual Qt3DCore::QTransform* create(Polyhedron* polyhedron) = 0;
private:
    Qt3DCore::QTransform* transform;
};
}


#endif // SIMPLETRANSFORMFACTORY_H
