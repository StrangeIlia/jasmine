#ifndef INITIALIZER_H
#define INITIALIZER_H

#include "View3D.h"

namespace bstu {
class AbstractViewInitializer : public QObject
{
    Q_OBJECT
private:
   View3D* _view;
public:
    AbstractViewInitializer(View3D* view) : QObject(view) {
       this->_view = view;
       connect(view, SIGNAL(show()), SLOT(init()));
    }
    inline View3D* view() const {
        return _view;
    }
protected slots:
    virtual void init() = 0;
};
}

#endif // INITIALIZER_H
