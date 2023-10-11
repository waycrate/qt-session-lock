#pragma once

#include <QtWaylandClient/private/qwaylandshellintegration_p.h>
#include <qwayland-ext-session-lock-v1.h>

namespace ExtSessionLockV1Qt {

class QWaylandExtSessionLockManagerIntegration
  : public QtWaylandClient::QWaylandShellIntegrationTemplate<QWaylandExtSessionLockManagerIntegration>
  , public QtWayland::ext_session_lock_manager_v1
{
public:
    QWaylandExtSessionLockManagerIntegration();
    ~QWaylandExtSessionLockManagerIntegration() override;

    QtWaylandClient::QWaylandShellSurface *
    createShellSurface(QtWaylandClient::QWaylandWindow *window) override;
};

}
