#ifndef ABSTRACTENTITYSET_H
#define ABSTRACTENTITYSET_H

#include <QObject>
#include <Qt3DCore>

namespace bstu {
using namespace Qt3DCore;
class AbstractEntitySet : public QObject {
    Q_OBJECT
public:
    explicit AbstractEntitySet(QObject *parrent = nullptr);
    virtual bool has(QEntity* key) = 0;
    virtual int count() const = 0;
public slots:
    /// Возращает истина, если такого ключа не было в коллекции
    bool append(QEntity* key);
    /// Возвращает истина, если ключ был в коллекции
    bool remove(QEntity* key);
signals:
    void appended(QEntity* key);
    void removed(QEntity* key);
protected:
    virtual bool _setAppend(QEntity* key) = 0;
    virtual bool _setRemove(QEntity* key) = 0;
};
}
#endif // ABSTRACTENTITYSET_H
