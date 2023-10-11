#include "qwaylandextsessionlockintegrationplugin.h"

#include "qwaylandextsessionlockmanagerintegration.h"
using namespace ExtSessionLockV1Qt;

QtWaylandClient::QWaylandShellIntegration *
QWaylandExtSessionLockIntegrationPlugin::create(const QString &key, const QStringList &paramList)
{
    Q_UNUSED(key);
    Q_UNUSED(paramList);
    return new QWaylandExtSessionLockManagerIntegration();
}
