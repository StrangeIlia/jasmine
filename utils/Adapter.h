#ifndef ADAPTER_H
#define ADAPTER_H

#include <QObject>

#include "collections/AbstractEntitySet.h"
#include "View3D.h"

namespace bstu {
class Adapter : public QObject {
    Q_OBJECT
public:
    explicit Adapter(View3D* view, AbstractEntitySet* set);
private slots:
    void addEntity(QEntity* entity);
    void removeEntity(QEntity* enity);
private:
    View3D* view;
    AbstractEntitySet* set;
};
}

#endif // ADAPTER_H
