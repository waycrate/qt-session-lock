#pragma once

#include <QtWaylandClient/private/qwaylandshellintegration_p.h>
#include <qwayland-ext-session-lock-v1.h>

namespace ExtSessionLockV1Qt {
class QWaylandExtLockSurface;
class QWaylandExtSessionLockManagerIntegration
  : public QtWaylandClient::QWaylandShellIntegrationTemplate<
      QWaylandExtSessionLockManagerIntegration>
  , public QtWayland::ext_session_lock_manager_v1
{
    Q_OBJECT
    friend QWaylandExtLockSurface;

public:
    QWaylandExtSessionLockManagerIntegration();
    ~QWaylandExtSessionLockManagerIntegration() override;

    QtWaylandClient::QWaylandShellSurface *
    createShellSurface(QtWaylandClient::QWaylandWindow *window) override;

signals:
    void requestLock();

private:
    QtWayland::ext_session_lock_v1 *m_lock;
};

}
