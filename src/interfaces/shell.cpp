#include "shell.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(SessionLock, "QtSessionLock");
using namespace ExtSessionLockV1Qt;

void
Shell::useExtSessionLock()
{
    const bool ret = qputenv("QT_WAYLAND_SHELL_INTEGRATION", "ext-sessionlock");
    if (!ret) {
        qCDebug(SessionLock) << "Unable to set QT_WAYLAND_SHELL_INTEGRATION=ext-sessionlock";
    }
}
