#ifndef INITIALIZER_H
#define INITIALIZER_H

#include "View3D.h"

#include "exceptions/NotImplementedException.hpp"

namespace bstu {
class AbstractViewInitializer : public QObject
{
    Q_OBJECT
protected:
    virtual void init(){
        throw NotImplementedException();
    }
private:
   View3D* _view;
   void nonNullInit() { /// В конструкторе метод init будет равен null
        if(_view->isActive())
            init();
   }
public:
    AbstractViewInitializer(View3D* view) : QObject(view) {
       this->_view = view;
       connect(view, SIGNAL(activeChanged()), this, SLOT(nonNullInit()));
    }
    inline View3D* view() const {
        return _view;
    }
};
}

#endif // INITIALIZER_H
