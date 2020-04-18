#ifndef ABSTRACTLOCKABLE_H
#define ABSTRACTLOCKABLE_H

#include <QObject>

/// Пока не используется (!)
namespace bstu {
class QT_DEPRECATED AbstractLockable {
public:
    virtual void lockForRead() = 0;
    virtual void lockForWrite() = 0;
    virtual bool tryLockForRead(int timeout = 0) = 0;
    virtual bool tryLockForWrite(int timeout = 0) = 0;
    virtual void unlockForRead() = 0;
    virtual void unlockForWrite() = 0;
};
}

#endif // ABSTRACTLOCKABLE_H
