#include "qwaylandextsessionlocksurface.h"
#include "src/qwaylandxdgactivationv1.h"
#include "window.h"

#include <QtWaylandClient/private/qwaylandinputdevice_p.h>
#include <QtWaylandClient/private/qwaylandshellsurface_p.h>
#include <QtWaylandClient/private/qwaylandsurface_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

static QtWaylandClient::QWaylandWindow *oldWindow = nullptr;

namespace ExtSessionLockV1Qt {

QWaylandExtLockSurface::QWaylandExtLockSurface(QWaylandExtSessionLockManagerIntegration *manager,
                                               QtWaylandClient::QWaylandWindow *window)
  : QtWaylandClient::QWaylandShellSurface(window)
  , QtWayland::ext_session_lock_surface_v1()
  , m_shell(manager)
  , m_window(window)
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
          m_isLocked   = true;
          auto surface = manager->m_lock->get_lock_surface(window->waylandSurface()->object(),
                                                           interface->get_wl_output());
          init(surface);
          m_surface = QtWayland::ext_session_lock_surface_v1::fromObject(surface);
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
    m_peddingSize = QSize(width, height);
    if (!m_configured) {
        m_configured = true;
        ack_configure(serial);
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
void
QWaylandExtLockSurface::setXdgActivationToken(const QString &token)
{
    m_activationToken = token;
}

void
QWaylandExtLockSurface::requestWindowStates(Qt::WindowStates states)
{
    if (states & Qt::WindowActive) {
        this->focusWindow();
    }
}

std::any
QWaylandExtLockSurface::surfaceRole() const
{
    if (m_surface) {
        return m_surface->object();
    }
    return {};
}

void
QWaylandExtLockSurface::focusWindow()
{
    auto wFocusWindow = m_window;
    wFocusWindow->display()->handleWindowActivated(wFocusWindow);
    if (wFocusWindow->display()->defaultInputDevice() &&
        wFocusWindow->display()->defaultInputDevice()->keyboard()) {
        wFocusWindow->display()->defaultInputDevice()->keyboard()->mFocus =
          wFocusWindow->waylandSurface();
    }
    if (oldWindow && oldWindow != wFocusWindow) {
        oldWindow->display()->handleWindowDeactivated(oldWindow);
    }
    oldWindow = wFocusWindow;
}

void
QWaylandExtLockSurface::requestXdgActivationToken(quint32 serial)
{
    QWaylandXdgActivationV1 *activation = m_shell->activation();
    if (!activation->isActive()) {
        return;
    }
    auto tokenProvider = activation->requestXdgActivationToken(
      window()->display(), window()->wlSurface(), serial, QString());

    connect(tokenProvider, &QWaylandXdgActivationTokenV1::done, this, [this](const QString &token) {
        Q_EMIT window()->xdgActivationTokenCreated(token);
    });
    connect(
      tokenProvider, &QWaylandXdgActivationTokenV1::done, tokenProvider, &QObject::deleteLater);
}

bool
QWaylandExtLockSurface::requestActivate()
{
    QWaylandXdgActivationV1 *activation = m_shell->activation();
    if (!activation->isActive()) {
        return false;
    }
    if (!m_activationToken.isEmpty()) {
        activation->activate(m_activationToken, window()->wlSurface());
        m_activationToken = {};
        return true;
    } else if (const auto token = qEnvironmentVariable("XDG_ACTIVATION_TOKEN"); !token.isEmpty()) {
        activation->activate(token, window()->wlSurface());
        qunsetenv("XDG_ACTIVATION_TOKEN");
        return true;
    } else {
        const auto focusWindow = QGuiApplication::focusWindow();
        const auto wlWindow =
          focusWindow && focusWindow->handle()
            ? static_cast<QtWaylandClient::QWaylandWindow *>(focusWindow->handle())
            : window();
        if (const auto seat = wlWindow->display()->lastInputDevice()) {
            const auto tokenProvider = activation->requestXdgActivationToken(
              wlWindow->display(), wlWindow->wlSurface(), seat->serial(), QString());
            connect(tokenProvider,
                    &QWaylandXdgActivationTokenV1::done,
                    this,
                    [this](const QString &token) {
                        m_shell->activation()->activate(token, window()->wlSurface());
                    });
            connect(tokenProvider,
                    &QWaylandXdgActivationTokenV1::done,
                    tokenProvider,
                    &QObject::deleteLater);
            return true;
        }
    }
    return false;
}

bool
QWaylandExtLockSurface::requestActivateOnShow()
{
    return requestActivate();
}

}
