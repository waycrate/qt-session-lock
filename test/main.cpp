/*
 *   SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "interfaces/shell.h"
#include "interfaces/window.h"

#include <QGuiApplication>
#include <QPainter>
#include <QRasterWindow>
#include <QTimer>
#include <QWindow>

#include <QMetaEnum>

#include <QtWaylandClient/private/qwaylandscreen_p.h>
#include <QtWaylandClient/private/qwaylandsurface_p.h>
#include <QtWaylandClient/private/qwaylandwindow_p.h>

using namespace ExtSessionLockV1Qt;

QStringList
enumsToStringList(QMetaEnum metaEnum)
{
    QStringList ret;
    ret.reserve(metaEnum.keyCount());
    for (int i = 0; i < metaEnum.keyCount(); ++i) {
        ret.append(metaEnum.key(i));
    }
    return ret;
}

template<typename T>
T
stringToEnum(QMetaEnum metaEnum, const QString &str)
{
    T ret               = {};
    const auto splitted = str.split(QLatin1Char('|'), Qt::SkipEmptyParts);
    for (const auto &value : splitted) {
        ret |= T(metaEnum.keyToValue(qPrintable(value)));
    }
    return ret;
}

class BasicWindow : public QRasterWindow
{
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.fillRect(QRect(0, 0, width(), height()), Qt::blue);
    }
};

int
main(int argc, char **argv)
{
    Shell::useExtSessionLock();

    QGuiApplication app(argc, argv);

    Window *w    = nullptr;
    auto screens = QGuiApplication::screens();
    QList<BasicWindow *> windows;
    for (auto screen : screens) {
        BasicWindow *window = new BasicWindow;
        auto waylandScreen  = dynamic_cast<QtWaylandClient::QWaylandScreen *>(screen->handle());
        w                   = Window::registerWindow(window, waylandScreen->output());

        windows.push_back(window);
    }
    for (auto window : windows) {
        window->show();
    }
    QTimer::singleShot(10000, &app, [w] {
        w->unlockScreen();
        QGuiApplication::quit();
    });

    return app.exec();
}
