#include "window.h"
#include "../qwaylandextsessionlockmanagerintegration.h"
#include <QMap>
#include <QPlatformSurfaceEvent>
#include <QtWaylandClient/private/qwaylandscreen_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <cassert>
Q_LOGGING_CATEGORY(SESSION_LOCK, "session lock")

namespace ExtSessionLockV1Qt {

static QMap<QWindow *, Window *> s_map;

Window *
Window::get(QWindow *window)
{
    if (!window) {
        return nullptr;
    }
    return s_map.value(window);
}

Window *
Window::registerWindowFromWlOutput(QWindow *window, ::wl_output *output)
{
    Window *w   = new Window;
    w->m_output = output;
    w->m_window = window;
    s_map.insert(window, w);
    return w;
}

Window *
Window::registerWindowFromQtScreen(QWindow *window, QScreen *screen)
{
    auto waylandScreen = dynamic_cast<QtWaylandClient::QWaylandScreen *>(screen->handle());
    Window *w          = new Window;
    w->m_output        = waylandScreen->output();
    w->m_window        = window;
    s_map.insert(window, w);
    window->installEventFilter(w);
    if (window->handle()) {
        w->initializeShell();
    }
    return w;
}
bool
Window::eventFilter(QObject *watched, QEvent *event)
{
    auto window = qobject_cast<QWindow *>(watched);
    if (!window) {
        return false;
    }
    if (event->type() == QEvent::PlatformSurface) {
        if (auto pse = static_cast<QPlatformSurfaceEvent *>(event);
            pse->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
            initializeShell();
        }
    }
    return false;
}

void
Window::initializeShell()
{
    auto window        = this->m_window;
    auto waylandWindow = dynamic_cast<QtWaylandClient::QWaylandWindow *>(window->handle());
    static QWaylandExtSessionLockManagerIntegration *shellIntegration = nullptr;
    if (!shellIntegration) {
        shellIntegration = new QWaylandExtSessionLockManagerIntegration();
        assert(waylandWindow->display() != nullptr);
        if (!shellIntegration->initialize(waylandWindow->display())) {
            delete shellIntegration;
            shellIntegration = nullptr;
            qCWarning(SESSION_LOCK)
              << "Failed to initialize layer-shell integration, possibly because compositor does "
                 "not support the layer-shell protocol";
        }
    }
    waylandWindow->setShellIntegration(shellIntegration);
}

void
Window::unlockScreen()
{
    Q_EMIT requestUnlock();
}

Window::~Window()
{
    s_map.remove(m_window);
}
}
