#pragma once
#include <QObject>
namespace ExtSessionLockV1Qt {

class Command : public QObject
{
    Q_OBJECT
public:
    static Command *instance();

    void unLockScreen();

signals:
    void requestUnlock();

private:
    explicit Command(QObject *parent = nullptr);
};
}
