#pragma once

#include "qwayland-ext-session-lock-v1.h"
#include "src/qwaylandextsessionlockmanagerintegration.h"
#include "window.h"

#include <QtWaylandClient/private/qwaylandshellsurface_p.h>

namespace ExtSessionLockV1Qt {
class QWaylandExtLockSurface
  : public QtWaylandClient::QWaylandShellSurface
  , public QtWayland::ext_session_lock_surface_v1
{
    Q_OBJECT
public:
    explicit QWaylandExtLockSurface(
      ExtSessionLockV1Qt::QWaylandExtSessionLockManagerIntegration *lockmanager,
      QtWaylandClient::QWaylandWindow *window);
    ~QWaylandExtLockSurface() override;

    bool isExposed() const override { return m_configured; }
    void applyConfigure() override;

private:
    void ext_session_lock_surface_v1_configure(uint32_t serial,
                                               uint32_t width,
                                               uint32_t height) override;

private:
    QSize m_peddingSize;
    bool m_configured = false;
    bool m_isLocked = false;
};
}
