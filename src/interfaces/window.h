#pragma once

#include <QObject>
#include <QWindow>
#include <QScreen>
#include <wayland-client.h>

namespace ExtSessionLockV1Qt {

class Window : public QObject
{
    Q_OBJECT

public:
    ~Window() override;

    static Window *get(QWindow *Window);

    static Window* registerWindowFromWlOutput(QWindow *window, ::wl_output *output);
    static Window* registerWindowFromQtScreen(QWindow *window, QScreen *screen);

    ::wl_output *get_wl_output() { return m_output; };

    void unlockScreen();
signals:
    void requestUnlock();

private:
    QWindow *m_window;
    ::wl_output *m_output;
};

}
