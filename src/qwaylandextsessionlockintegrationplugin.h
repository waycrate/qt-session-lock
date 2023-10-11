#pragma once

#include <QtWaylandClient/private/qwaylandshellintegrationplugin_p.h>

class QWaylandExtSessionLockIntegrationPlugin
  : public QtWaylandClient::QWaylandShellIntegrationPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QWaylandShellIntegrationFactoryInterface_iid FILE "ext-sessionlock.json")
public:
    QWaylandExtSessionLockIntegrationPlugin() {}

    QtWaylandClient::QWaylandShellIntegration *
    create(const QString &key, const QStringList &paramList) override;
};
