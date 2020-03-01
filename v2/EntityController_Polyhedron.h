#ifndef IENTITYCONTROLLER_H
#define IENTITYCONTROLLER_H

#include <QObject>

class IEntityController : public QObject
{
    Q_OBJECT
public:
    explicit IEntityController(QObject *parent = nullptr);

signals:

};

#endif // IENTITYCONTROLLER_H
