#pragma once

#include <QtWaylandClient/private/qwaylandshellintegration_p.h>
#include <qwayland-ext-session-lock-v1.h>
#include <qwaylandclientextension.h>

class QWaylandXdgActivationV1;
namespace ExtSessionLockV1Qt {
class QWaylandExtLockSurface;
class QWaylandExtSessionLock;
class QWaylandExtSessionLockManagerIntegration
  : public QtWaylandClient::QWaylandShellIntegrationTemplate<
      QWaylandExtSessionLockManagerIntegration>
  , public QtWayland::ext_session_lock_manager_v1
{
    Q_OBJECT
    friend QWaylandExtLockSurface;

public:
    QWaylandExtSessionLockManagerIntegration();
    ~QWaylandExtSessionLockManagerIntegration() override;

    QtWaylandClient::QWaylandShellSurface *
    createShellSurface(QtWaylandClient::QWaylandWindow *window) override;
    QWaylandXdgActivationV1 *activation() const { return m_xdgActivation.data(); }

public slots:
    void tryLockScreen();
    void tryUnlockScreen();
signals:
    void requestLock();

private:
    QWaylandExtSessionLock *m_lock;

private:
    QScopedPointer<QWaylandXdgActivationV1> m_xdgActivation;
};

class QWaylandExtSessionLock
  : public QObject
  , public QtWayland::ext_session_lock_v1

{
    Q_OBJECT
public:
    explicit QWaylandExtSessionLock(QWaylandExtSessionLockManagerIntegration *manager);
    ~QWaylandExtSessionLock() override;

    void ext_session_lock_v1_locked() override;
    void ext_session_lock_v1_finished() override;

private:
    QWaylandExtSessionLockManagerIntegration *m_manager;
};

}
