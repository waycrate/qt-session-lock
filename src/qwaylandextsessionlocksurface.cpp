#include "qwaylandextsessionlocksurface.h"
#include "wayland-ext-session-lock-v1-client-protocol.h"
#include "window.h"

#include <QtWaylandClient/private/qwaylandscreen_p.h>
#include <QtWaylandClient/private/qwaylandshellsurface_p.h>
#include <QtWaylandClient/private/qwaylandsurface_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

namespace ExtSessionLockV1Qt {

QWaylandExtLockSurface::QWaylandExtLockSurface(QtWayland::ext_session_lock_v1 *lock,
                                               QtWaylandClient::QWaylandWindow *window)
  : QtWaylandClient::QWaylandShellSurface(window)
  , QtWayland::ext_session_lock_surface_v1()
{
    QMetaObject::invokeMethod(
      this,
      [window, lock, this] {
          ExtSessionLockV1Qt::Window *inteface = Window::get(window->window());
          Q_ASSERT(inteface);

          init(
            lock->get_lock_surface(window->waylandSurface()->object(), inteface->get_wl_output()));
          connect(inteface, &Window::requestUnlock, this, [lock] { lock->unlock_and_destroy(); });
      },
      Qt::QueuedConnection);
}

QWaylandExtLockSurface::~QWaylandExtLockSurface()
{
    destroy();
}

void
QWaylandExtLockSurface::ext_session_lock_surface_v1_configure(uint32_t serial,
                                                              uint32_t width,
                                                              uint32_t height)
{
    ack_configure(serial);
    m_peddingSize = QSize(width, height);
    if (!m_configured) {
        m_configured = true;
        window()->resizeFromApplyConfigure(m_peddingSize);
        window()->handleExpose(QRect(QPoint(), m_peddingSize));
    } else {
        window()->applyConfigureWhenPossible();
    }
}

void
QWaylandExtLockSurface::applyConfigure()
{
    window()->resizeFromApplyConfigure(m_peddingSize);
}
}
