#include "qwaylandextsessionlockmanagerintegration.h"
#include "qwaylandextsessionlocksurface.h"
#include "qwaylandxdgactivationv1.h"
#include "wayland-ext-session-lock-v1-client-protocol.h"

#include "interfaces/command.h"
#include <qwaylandclientextension.h>

namespace ExtSessionLockV1Qt {
QWaylandExtSessionLockManagerIntegration::QWaylandExtSessionLockManagerIntegration()
  : QWaylandShellIntegrationTemplate<QWaylandExtSessionLockManagerIntegration>(1)
  , QtWayland::ext_session_lock_manager_v1()
  , m_xdgActivation(new QWaylandXdgActivationV1)
{
    connect(Command::instance(),
            &Command::requestUnlock,
            this,
            &QWaylandExtSessionLockManagerIntegration::tryUnlockScreen);

    connect(Command::instance(),
            &Command::requestLock,
            this,
            &QWaylandExtSessionLockManagerIntegration::tryLockScreen);
}

void
QWaylandExtSessionLockManagerIntegration::tryLockScreen()
{
    if (!m_lock) {
        // NOTE: if no lock, wait the callback, then send the signal
        m_lock = new QWaylandExtSessionLock(this);
    } else {
        // NOTE: just tell it is ok to generate a new window
        Q_EMIT requestLock();
    }
}
void
QWaylandExtSessionLockManagerIntegration::tryUnlockScreen()
{
    if (m_lock->isInitialized()) {
        m_lock->unlock_and_destroy();
    }
    if (m_lock) {
        m_lock->deleteLater();
        m_lock = nullptr;
    }
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
    return new QWaylandExtLockSurface(this, window);
}

QWaylandExtSessionLock::QWaylandExtSessionLock(QWaylandExtSessionLockManagerIntegration *manager)
  : QObject(manager)
  , QtWayland::ext_session_lock_v1(manager->lock())
  , m_manager(manager)
{
}

void
QWaylandExtSessionLock::ext_session_lock_v1_locked()
{
    Q_EMIT this->m_manager->requestLock();
}

void
QWaylandExtSessionLock::ext_session_lock_v1_finished()
{
    this->deleteLater();
}

QWaylandExtSessionLock::~QWaylandExtSessionLock()
{
    if (object() && ext_session_lock_v1_get_version(object()) >=
                      EXT_SESSION_LOCK_MANAGER_V1_DESTROY_SINCE_VERSION) {
        ext_session_lock_v1_destroy(object());
    }
}
}
