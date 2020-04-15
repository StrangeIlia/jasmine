#ifndef ABSTRACTSET_H
#define ABSTRACTSET_H

#include "extensions/PolyhedronExtension.h"

#include <QObject>

namespace bstu {
template<class T>
class AbstractSet : public QObject {
    Q_OBJECT
public:
    explicit AbstractSet(QObject *parrent = nullptr);
    virtual bool has(T* key) = 0;
public slots:
    /// Возращает истина, если такого ключа не было в коллекции
    bool append(T* key);
    /// Возвращает истина, если ключ был в коллекции
    bool remove(T* key);
signals:
    void appended(T* key);
    void removed(T* key);
protected:
    virtual bool _setAppend(T* key) = 0;
    virtual bool _setRemove(T* key) = 0;
};
}

#endif // ABSTRACTSET_H
