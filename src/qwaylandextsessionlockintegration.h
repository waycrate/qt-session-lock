#pragma once

#include <QtWaylandClient/private/qwaylandshellintegration_p.h>
#include <qwayland-ext-session-lock-v1.h>

namespace ExtSessionLockV1Qt {

class QWaylandSessionLockIntegration
  : public QtWaylandClient::QWaylandShellIntegrationTemplate<QWaylandSessionLockIntegration>
  , public QtWayland::ext_session_lock_v1
{
public:
    QWaylandSessionLockIntegration();
    ~QWaylandSessionLockIntegration() override;

    QtWaylandClient::QWaylandShellSurface *
    createShellSurface(QtWaylandClient::QWaylandWindow *window) override;
};

}
