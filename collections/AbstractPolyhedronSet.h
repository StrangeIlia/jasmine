#ifndef ABSTRACTPOLYHEDRONSET_H
#define ABSTRACTPOLYHEDRONSET_H

#include <QObject>

#include "extensions/PolyhedronExtension.h"

namespace bstu {
class AbstractPolyhedronSet : public QObject {
    Q_OBJECT
public:
    explicit AbstractPolyhedronSet(QObject *parrent = nullptr);
    virtual bool has(PolyhedronExtension* key) = 0;
    virtual int count() const = 0;
public slots:
    /// Возращает истина, если такого ключа не было в коллекции
    bool append(PolyhedronExtension* key);
    /// Возвращает истина, если ключ был в коллекции
    bool remove(PolyhedronExtension* key);
signals:
    void appended(PolyhedronExtension* key);
    void removed(PolyhedronExtension* key);
protected:
    virtual bool _setAppend(PolyhedronExtension* key) = 0;
    virtual bool _setRemove(PolyhedronExtension* key) = 0;
};
}
#endif // ABSTRACTPOLYHEDRONSET_H
