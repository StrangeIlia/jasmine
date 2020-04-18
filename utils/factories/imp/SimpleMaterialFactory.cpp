#include "SimpleMaterialFactory.h"

namespace bstu {
SimpleMaterialFactory::SimpleMaterialFactory(QNode* root)
{
    QEntity* fictitious = new QEntity(root);
    QPhongAlphaMaterial* material = new QPhongAlphaMaterial();
    material->setShareable(true);
    material->setAmbient(QColor::fromRgbF(50.0 / 255.0, 130.0 / 255.0, 200.0 / 255.0, 1));
    material->setShininess(0.8f);
    material->setAlpha(1.0f);
    fictitious->addComponent(material);
    this->material = material;
}

QMaterial* SimpleMaterialFactory::create(Polyhedron* ) {
    return material;
}

}
