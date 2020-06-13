#include "SimpleMaterialFactory.h"

namespace bstu {
SimpleMaterialFactory::SimpleMaterialFactory(QNode* root)
{
    QEntity* fictitious = new QEntity(root);
    QPhongMaterial* material = new QPhongMaterial(fictitious);
    material->setShareable(true);
    material->setAmbient(QColor::fromRgbF(50.0 / 255.0, 130.0 / 255.0, 200.0 / 255.0, 1));
    material->setShininess(0.8f);
    //material->setAlpha(1.0f);
    fictitious->addComponent(material);
    this->material = material;
    QEffect *effect = material->effect();
    for (auto t : effect->techniques()) {
        for (auto rp : t->renderPasses()) {
            auto pointSize = new QPointSize();
            pointSize->setSizeMode(QPointSize::SizeMode::Fixed);
            pointSize->setValue(10.0f);
            rp->addRenderState(pointSize);
        }
    }
}

QMaterial* SimpleMaterialFactory::create(Polyhedron* ) {
    return material;
}

}
