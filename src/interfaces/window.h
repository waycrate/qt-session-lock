#pragma once

#include "sessionlockqtinterface_export.h"
#include <QObject>
#include <QScreen>
#include <QWindow>
#include <wayland-client.h>

namespace ExtSessionLockV1Qt {

class SESSIONLOCKQTINTERFACE_EXPORT Window : public QObject
{
    Q_OBJECT

public:
    ~Window() override;

    static Window *get(QWindow *Window);

    static Window *registerWindowFromWlOutput(QWindow *window, ::wl_output *output);
    static Window *registerWindowFromQtScreen(QWindow *window, QScreen *screen);

    ::wl_output *get_wl_output() { return m_output; };

    void unlockScreen();
    bool eventFilter(QObject *watched, QEvent *event) override;
signals:
    void requestUnlock();

private:
    void initializeShell();

private:
    QWindow *m_window;
    ::wl_output *m_output;
};

}
