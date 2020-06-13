#include "SelectorInitializer.h"

namespace bstu {

SelectorInitializer::Action::~Action() {}

SelectorInitializer::SelectorInitializer(View3D *view, std::vector<Action*> _list) : AbstractViewInitializer(view)
{
    QEntity* fictitious = new QEntity(view->rootEntity());
    QPhongMaterial* selectedMaterial = new QPhongMaterial(fictitious);
    selectedMaterial->setShareable(true);

    fictitious->addComponent(selectedMaterial);
    list = std::move(_list);
}

SelectorInitializer::~SelectorInitializer() {
    for(SelectorInitializer::Action* action : list)
        delete action;
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
    unselectDisable(false);
    if(event->entity() != nullptr && !equals) {
        selectedEntity = event->entity();
        connect(selectedEntity, SIGNAL(destroyed(QObject*)), this, SLOT(unselectDelete(QObject*)));
        connect(selectedEntity, SIGNAL(enabledChanged(bool)), this, SLOT(unselectDisable(bool)));
        for(Action* action : list) {
            action->entitySelected(selectedEntity);
        }
    }
}

void SelectorInitializer::unselectDelete(QObject* ) {
    if(selectedEntity != nullptr) {
        disconnect(selectedEntity, SIGNAL(destroyed(QObject*)), this, SLOT(unselectDelete(QObject*)));
        for(Action* action : list) {
            action->entityUnselected(selectedEntity);
        }
        selectedEntity = nullptr;
    }
}

void SelectorInitializer::unselectDisable(bool v) {
    if(v == false){
        if(selectedEntity != nullptr) {
            disconnect(selectedEntity, SIGNAL(enabledChanged(bool)), this, SLOT(unselectDisable(bool)));
            unselectDelete(nullptr);
        }
    }
}

}

