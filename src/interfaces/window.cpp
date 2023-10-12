#include "window.h"
#include <QMap>

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
Window::registerWindow(QWindow *window, ::wl_output *output)
{
    Window *w   = new Window;
    w->m_output = output;
    w->m_window = window;
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
