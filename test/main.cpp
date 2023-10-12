/*
 *   SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "interfaces/command.h"
#include "interfaces/shell.h"
#include "interfaces/window.h"

#include <QGuiApplication>
#include <QPainter>
#include <QRasterWindow>
#include <QTimer>
#include <QWindow>

#include <QMetaEnum>

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
public:
    explicit BasicWindow(QColor color)
      : m_color(color)
    {
    }

private:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.fillRect(QRect(0, 0, width(), height()), m_color);
    }

private:
    QColor m_color;
};

int
main(int argc, char **argv)
{
    Shell::useExtSessionLock();

    QGuiApplication app(argc, argv);

    auto screens = QGuiApplication::screens();
    QList<BasicWindow *> windows;
    int i = 0;
    for (auto screen : screens) {
        QColor color = Qt::blue;
        if (i % 2 == 0) {
            color = Qt::darkYellow;
        }
        BasicWindow *window = new BasicWindow(color);
        Window::registerWindowFromQtScreen(window, screen);

        windows.push_back(window);
        i += 1;
    }
    for (auto window : windows) {
        window->show();
    }
    QTimer::singleShot(2000, &app, [] {
        Command::instance()->unLockScreen();
        QGuiApplication::quit();
    });

    return app.exec();
}
