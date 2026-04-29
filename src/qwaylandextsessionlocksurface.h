#pragma once

#include "qwayland-ext-session-lock-v1.h"
#include "src/qwaylandextsessionlockmanagerintegration.h"

#include <QtWaylandClient/private/qwaylandshellsurface_p.h>

namespace ExtSessionLockV1Qt {
class QWaylandExtLockSurface
  : public QtWaylandClient::QWaylandShellSurface
  , public QtWayland::ext_session_lock_surface_v1
{
    Q_OBJECT
public:
    explicit QWaylandExtLockSurface(QWaylandExtSessionLockManagerIntegration *lockmanager,
                                    QtWaylandClient::QWaylandWindow *window);
    ~QWaylandExtLockSurface() override;

    bool isExposed() const override { return m_configured; }
    void applyConfigure() override;
    bool requestActivate() override;
    bool requestActivateOnShow() override;
    void setXdgActivationToken(const QString &token) override;
    void requestXdgActivationToken(quint32 serial) override;

    void requestWindowStates(Qt::WindowStates states) override;

    std::any surfaceRole() const override;

private:
    void ext_session_lock_surface_v1_configure(uint32_t serial,
                                               uint32_t width,
                                               uint32_t height) override;
    void sendExpose();

    void focusWindow();

private:
    QSize m_peddingSize;
    QWaylandExtSessionLockManagerIntegration *m_shell;
    bool m_configured = false;
    bool m_isLocked   = false;
    QString m_activationToken;
    QtWaylandClient::QWaylandWindow *m_window;
    QtWayland::ext_session_lock_surface_v1 *m_surface;
};
}
