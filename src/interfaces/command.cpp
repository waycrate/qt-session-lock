#include "command.h"
#include "../qwaylandextsessionlockmanagerintegration.h"
#include "window.h"
using namespace ExtSessionLockV1Qt;

static Command *BACKEND_INSTANCE = nullptr;

Command::Command(QObject *parent)
  : QObject(parent)
{
}

Command *
Command::instance()
{
    if (!BACKEND_INSTANCE) {
        BACKEND_INSTANCE = new Command;
    }
    return BACKEND_INSTANCE;
}

void
Command::unLockScreen()
{
    auto manager = Window::sessionLockManager();
    if (manager) {
        manager->tryUnlockScreen();
    } else {
        Q_EMIT requestUnlock();
    }
}

void
Command::LockScreen()
{
    auto manager = Window::sessionLockManager();
    if (manager) {
        manager->tryLockScreen();
    } else {
        Q_EMIT requestLock();
    }
}
