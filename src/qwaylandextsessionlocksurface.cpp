#include "qwaylandextsessionlocksurface.h"
#include "window.h"

#include <QtWaylandClient/private/qwaylandshellsurface_p.h>
#include <QtWaylandClient/private/qwaylandsurface_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

namespace ExtSessionLockV1Qt {

QWaylandExtLockSurface::QWaylandExtLockSurface(QWaylandExtSessionLockManagerIntegration *manager,
                                               QtWaylandClient::QWaylandWindow *window)
  : QtWaylandClient::QWaylandShellSurface(window)
  , QtWayland::ext_session_lock_surface_v1()
{
    ExtSessionLockV1Qt::Window *interface = Window::get(window->window());
    window->waylandSurface()->commit(); // ensure surface is committed, then can be locked safely
    Q_ASSERT(interface);
    connect(
      manager,
      &QWaylandExtSessionLockManagerIntegration::requestLock,
      this,
      [window, manager, interface, this] {
          if (m_isLocked) {
              return;
          }
          m_isLocked = true;
          init(manager->m_lock->get_lock_surface(window->waylandSurface()->object(),
                                                 interface->get_wl_output()));
          connect(interface, &Window::requestUnlock, this, [manager] {
              manager->m_lock->unlock_and_destroy();
          });
      },
      Qt::QueuedConnection);
}

QWaylandExtLockSurface::~QWaylandExtLockSurface()
{
    if (isInitialized()) {
        destroy();
    }
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
        this->sendExpose();
    } else {
        window()->applyConfigureWhenPossible();
    }
}

void
QWaylandExtLockSurface::applyConfigure()
{
    window()->resizeFromApplyConfigure(m_peddingSize);
}

void
QWaylandExtLockSurface::sendExpose()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    window()->handleExpose(QRect(QPoint(), m_pendingSize));
#elif QT_VERSION < QT_VERSION_CHECK(6, 9, 0)
    window()->sendRecursiveExposeEvent();
#else
    window()->updateExposure();
#endif
}
}
