#include "ActionChangeColor.h"

namespace bstu {
ActionChangeColor::ActionChangeColor(QMaterial* material) : selectedMaterial(material)
{
    selectedMaterial->setShareable(true);
}

void ActionChangeColor::entitySelected(QEntity* entity) {
    oldMaterial = entity->componentsOfType<QMaterial>().first();
    entity->removeComponent(oldMaterial);
    entity->addComponent(selectedMaterial);
    selectedMaterial->moveToThread(entity->thread());
}

void ActionChangeColor::entityUnselected(QEntity* entity) {
    if(oldMaterial != nullptr) {
        // Проверка, что объект не удален
        if(entity->components().contains(selectedMaterial)) {
            entity->removeComponent(selectedMaterial);
            entity->addComponent(oldMaterial);
        }
        oldMaterial = nullptr;
    }
}

}
