#include "qwaylandextsessionlocksurface.h"

namespace ExtSessionLockV1Qt {

QWaylandExtLockSurface::QWaylandExtLockSurface(QtWayland::ext_session_lock_manager_v1 *lock,
                                               QtWaylandClient::QWaylandWindow *window)
  : QtWaylandClient::QWaylandShellSurface(window)
  , QtWayland::ext_session_lock_surface_v1()
{
}

}
