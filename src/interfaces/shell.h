#pragma once

#include "sessionlockqtinterface_export.h"
#include <QObject>

namespace ExtSessionLockV1Qt {
class SESSIONLOCKQTINTERFACE_EXPORT Shell
{
public:
    SESSIONLOCKQTINTERFACE_DEPRECATED_EXPORT
    static void useExtSessionLock();
};
}
