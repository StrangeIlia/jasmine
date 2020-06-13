#ifndef ACTIONCHANGECOLOR_H
#define ACTIONCHANGECOLOR_H

#include "initializers/SelectorInitializer.h"

namespace bstu {

class ActionChangeColor : public SelectorInitializer::Action
{
public:
    // Цвет должен быть привязан к фиктивной сущности!!!
    ActionChangeColor(QMaterial* material);

    void entitySelected(QEntity* entity) override;
    void entityUnselected(QEntity* entity) override;
private:
    QMaterial* oldMaterial = nullptr;
    QMaterial* selectedMaterial;
};

}


#endif // ACTIONCHANGECOLOR_H
