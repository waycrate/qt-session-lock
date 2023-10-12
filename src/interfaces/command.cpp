#include "command.h"

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
    Q_EMIT requestUnlock();
}
