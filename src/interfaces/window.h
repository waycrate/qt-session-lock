#pragma once

#include <QObject>
#include <QWindow>
#include <wayland-client.h>

namespace ExtSessionLockV1Qt {

class Window : public QObject
{
    Q_OBJECT

public:
    ~Window() override;

    static Window *get(QWindow *Window);

    static void registWindow(QWindow *window, ::wl_output *output);

    ::wl_output *get_wl_output() { return m_output; };

private:
    QWindow *m_window;
    ::wl_output *m_output;
};

}
