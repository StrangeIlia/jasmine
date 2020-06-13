#ifndef ACTIONPOINTSGRID_H
#define ACTIONPOINTSGRID_H

#include "initializers/SelectorInitializer.h"

#include "utils/SmartComponent.h"
#include "utils/factories/imp/PointsGeometryFactory.h"

namespace bstu {

class ActionPointsGrid : public SelectorInitializer::Action
{
public:
    // Цвет должен быть привязан к фиктивной сущности!!!
    ActionPointsGrid(QMaterial* pointColor);

    void entitySelected(QEntity* entity) override;
    void entityUnselected(QEntity* entity) override;
private:
    QMaterial* color;

    QEntity* processedEntity = nullptr;
};

}
#endif // ACTIONPOINTSGRID_H
