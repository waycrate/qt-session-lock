#include "qwaylandextsessionlockmanagerintegration.h"
#include "qwaylandextsessionlocksurface.h"
#include "wayland-ext-session-lock-v1-client-protocol.h"

namespace ExtSessionLockV1Qt {
QWaylandExtSessionLockManagerIntegration::QWaylandExtSessionLockManagerIntegration()
  : QWaylandShellIntegrationTemplate<QWaylandExtSessionLockManagerIntegration>(1)
  , m_lock(new QtWayland::ext_session_lock_v1())
{
}

QWaylandExtSessionLockManagerIntegration::~QWaylandExtSessionLockManagerIntegration()
{
    if (object() && ext_session_lock_manager_v1_get_version(object()) >=
                      EXT_SESSION_LOCK_MANAGER_V1_DESTROY_SINCE_VERSION) {
        ext_session_lock_manager_v1_destroy(object());
    }
}

QtWaylandClient::QWaylandShellSurface *
QWaylandExtSessionLockManagerIntegration::createShellSurface(
  QtWaylandClient::QWaylandWindow *window)
{
    if (!m_lock->isInitialized()) {
        m_lock->init(lock());
    }

    return new QWaylandExtLockSurface(m_lock, window);
}
}
