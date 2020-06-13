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
    class Action {
    public:
        virtual ~Action();
        virtual void entitySelected(QEntity* entity) = 0;
        virtual void entityUnselected(QEntity* entity) = 0;
    };

    /// За освобождение памяти Action* отвечает селектор
    explicit SelectorInitializer(View3D *view, std::vector<Action*> list);
    ~SelectorInitializer();

private slots:
    void select(Qt3DRender::QPickEvent* );
    void unselectDelete(QObject* );
    void unselectDisable(bool );
private:
    void init() override;

    QEntity* selectedEntity = nullptr;
    std::vector<Action*> list;
};
}


#endif // SELECTOR_H
