#include "SelectorInitializer.h"

namespace bstu {
SelectorInitializer::SelectorInitializer(View3D *view) : AbstractViewInitializer(view)
{
    QEntity* fictitious = new QEntity(view->rootEntity());
    QPhongAlphaMaterial* selectedMaterial = new QPhongAlphaMaterial(fictitious);
    selectedMaterial->setShareable(true);
    selectedMaterial->setShininess(0.8f);
    selectedMaterial->setDiffuse(Qt::red);
    this->selectedMaterial = selectedMaterial;
    fictitious->addComponent(selectedMaterial);
}

void SelectorInitializer::init() {
    QRenderSettings* render_settings = view()->renderSettings();
    QPickingSettings* picking_settings = render_settings->pickingSettings();
    picking_settings->setPickMethod(QPickingSettings::PrimitivePicking);

    QObjectPicker* picker = new QObjectPicker(view()->rootEntity());
    picker->setEnabled(true);
    picker->setHoverEnabled(true);
    view()->rootEntity()->addComponent(picker);

    connect(picker, SIGNAL(clicked(Qt3DRender::QPickEvent*)), this, SLOT(select(Qt3DRender::QPickEvent*)));
}

void SelectorInitializer::select(Qt3DRender::QPickEvent* event) {
    if(event->button() != Qt3DRender::QPickEvent::LeftButton) return;
    bool equals = selectedEntity == event->entity();
    if(selectedEntity != nullptr) {
        selectedEntity->removeComponent(selectedMaterial);
        selectedEntity->addComponent(oldMaterial);
        selectedEntity = nullptr;
    }
    if(event->entity() != nullptr && !equals) {
        oldMaterial = event->entity()->componentsOfType<QMaterial>().first();
        event->entity()->removeComponent(oldMaterial);
        event->entity()->addComponent(selectedMaterial);
        selectedMaterial->moveToThread(event->entity()->thread());
        selectedEntity = event->entity();
    }
}

}

