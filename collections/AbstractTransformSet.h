#ifndef ABSTRACTTRANSFORMSET_H
#define ABSTRACTTRANSFORMSET_H

#include <QObject>
#include <Qt3DCore>

/*
 * QObject не поддерживает шаблонных наследников,
 * поэтому для каждого типа пришлось делать свой класс
*/

namespace bstu {
class AbstractTransformSet : public QObject {
    Q_OBJECT
public:
    explicit AbstractTransformSet(QObject *parrent = nullptr);
    virtual bool has(Qt3DCore::QTransform* key) = 0;
    virtual int count() const = 0;
public slots:
    /// Возращает истина, если такого ключа не было в коллекции
    bool append(Qt3DCore::QTransform* key);
    /// Возвращает истина, если ключ был в коллекции
    bool remove(Qt3DCore::QTransform* key);
signals:
    void appended(Qt3DCore::QTransform* key);
    void removed(Qt3DCore::QTransform* key);
protected:
    virtual bool _setAppend(Qt3DCore::QTransform* key) = 0;
    virtual bool _setRemove(Qt3DCore::QTransform* key) = 0;
};
}

#endif // ABSTRACTTRANSFORMSET_H
