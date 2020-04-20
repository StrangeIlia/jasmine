#include "SimpleTransformFactory.h"

namespace bstu {
SimpleTransformFactory::SimpleTransformFactory(QNode* root)
{
    QEntity* fictitious = new QEntity(root);
    Qt3DCore::QTransform* transform = new Qt3DCore::QTransform(fictitious);
    transform->setShareable(true);
    fictitious->addComponent(transform);
    this->transform = transform;
}

Qt3DCore::QTransform* SimpleTransformFactory::create(Polyhedron* ) {
    return transform;
}

}

