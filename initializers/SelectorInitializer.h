#ifndef SELECTOR_H
#define SELECTOR_H

#include <QObject>

#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>

#include "View3D.h"
#include "AbstractViewInitializer.hpp"

namespace bstu {
using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;
class SelectorInitializer : public AbstractViewInitializer
{
    Q_OBJECT
public:
    explicit SelectorInitializer(View3D *view);
private slots:
    void select(QPickEvent* );
private:
    void init() override;

    QMaterial* oldMaterial;
    QEntity* selectedEntity = nullptr;

    QMaterial* selectedMaterial;
};
}


#endif // SELECTOR_H
