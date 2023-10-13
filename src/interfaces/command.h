#pragma once
#include <QObject>
namespace ExtSessionLockV1Qt {

class Command : public QObject
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
