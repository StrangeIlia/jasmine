#ifndef VIEW3D_H
#define VIEW3D_H

#include <QVector>
#include <QWidget>

#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender>

namespace bstu {

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

/// View-шка для отображения объектов
class View3D : public Qt3DWindow
{
    Q_OBJECT
public:
    explicit View3D(QWidget* parent = nullptr);

    QEntity* rootEntity();
private:
    QEntity* _rootEntity = nullptr;
};

}

#endif // VIEW3D_H
