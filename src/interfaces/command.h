#pragma once

#include "sessionlockqtinterface_export.h"
#include <QObject>
namespace ExtSessionLockV1Qt {

class SESSIONLOCKQTINTERFACE_EXPORT Command : public QObject
{
    Q_OBJECT
public:
    static Command *instance();

    void unLockScreen();
    void LockScreen();

signals:
    void requestUnlock();
    void requestLock();

private:
    explicit Command(QObject *parent = nullptr);
};
}
