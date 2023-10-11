#include "qwaylandextsessionlockintegration.h"
#include "qwaylandextsessionlocksurface.h"
#include "wayland-ext-session-lock-v1-client-protocol.h"

namespace ExtSessionLockV1Qt {
using QtWaylandClient::QWaylandShellIntegrationTemplate;
QWaylandExtSessionLockIntegration::QWaylandExtSessionLockIntegration()
  : QWaylandShellIntegrationTemplate<QWaylandExtSessionLockIntegration>(1)
{
}

QWaylandExtSessionLockIntegration::~QWaylandExtSessionLockIntegration()
{
    if (object() &&
        ext_session_lock_v1_get_version(object()) >= EXT_SESSION_LOCK_V1_DESTROY_SINCE_VERSION) {
        ext_session_lock_v1_destroy(object());
    }
}

QtWaylandClient::QWaylandShellSurface *
QWaylandExtSessionLockIntegration::createShellSurface(QtWaylandClient::QWaylandWindow *window)
{
    return new QWaylandExtLockSurface(this, window);
}
}
