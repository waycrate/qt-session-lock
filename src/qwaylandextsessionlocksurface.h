#pragma once

#include "qwayland-ext-session-lock-v1.h"

#include <QtWaylandClient/private/qwaylandshellsurface_p.h>

namespace ExtSessionLockV1Qt {
class QWaylandExtLockSurface
  : public QtWaylandClient::QWaylandShellSurface
  , public QtWayland::ext_session_lock_surface_v1
{
    Q_OBJECT
public:
    explicit QWaylandExtLockSurface(QtWayland::ext_session_lock_manager_v1 *lock,
                                    QtWaylandClient::QWaylandWindow *window);
    ~QWaylandExtLockSurface() = default;
};
}
