#ifndef INITIALIZER_H
#define INITIALIZER_H

#include "View3D.h"

#include "exceptions/NotImplementedException.hpp"

namespace bstu {
class AbstractViewInitializer : public QObject
{
    Q_OBJECT
protected:
    virtual void init() = 0;
private:
   View3D* _view;
private slots:
   void nonNullInit(bool visible) { /// В конструкторе метод init будет равен null
        if(visible)
            init();
   }
public:
    AbstractViewInitializer(View3D* view) : QObject(view) {
       this->_view = view;
       connect(view, SIGNAL(visibleChanged(bool)), this, SLOT(nonNullInit(bool)));
    }
    inline View3D* view() const {
        return _view;
    }
};
}

#endif // INITIALIZER_H
