#include "window.h"
#include <QMap>
#include <QtWaylandClient/private/qwaylandscreen_p.h>

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
    return w;
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
