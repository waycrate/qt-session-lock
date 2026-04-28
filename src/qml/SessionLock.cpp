#include "SessionLock.h"
#include <QDebug>
#include <SessionLockQt/command.h>
#include <SessionLockQt/window.h>
#include <private/qwaylandinputdevice_p.h>
#include <private/qwaylandscreen_p.h>
#include <private/qwaylandsurface_p.h>
#include <private/qwaylandwindow_p.h>

static QtWaylandClient::QWaylandWindow *oldWindow = nullptr;

WlSessionLock::WlSessionLock(QObject *parent)
  : QObject(parent)
{
    auto *app    = QCoreApplication::instance();
    auto *guiApp = qobject_cast<QGuiApplication *>(app);
    if (guiApp != nullptr) {
        connect(guiApp, &QGuiApplication::screenAdded, this, &WlSessionLock::onScreenAdded);
        connect(guiApp, &QGuiApplication::screenRemoved, this, &WlSessionLock::onScreenRemoved);
    }
}

void
WlSessionLock::initScreens()
{
    auto screens = QGuiApplication::screens();
    screens.removeIf([](QScreen *screen) {
        if (dynamic_cast<QtWaylandClient::QWaylandScreen *>(screen->handle()) == nullptr) {
            qDebug() << "Not creating lock surface for screen" << screen
                     << "as it is not backed by a wayland screen.";

            return true;
        }

        return false;
    });
    for (auto *screen : screens) {
        if (!m_surfaces.contains(screen)) {
            auto *instanceObj = this->mSurfaceComponent->create(
              QQmlEngine::contextForObject(this->mSurfaceComponent));
            auto *instance = qobject_cast<WlSessionLockSurface *>(instanceObj);

            if (instance == nullptr) {
                qWarning()
                  << "WlSessionLock.surface does not create a WlSessionLockSurface. Aborting lock.";
                if (instanceObj != nullptr)
                    instanceObj->deleteLater();
                return;
            }

            instance->setParent(this);
            instance->setScreen(screen);

            m_surfaces[screen] = instance;
        }
    }
}

QQmlComponent *
WlSessionLock::surfaceComponent() const
{
    return this->mSurfaceComponent;
}

void
WlSessionLock::setSurfaceComponent(QQmlComponent *surfaceComponent)
{
    if (this->mSurfaceComponent != nullptr)
        this->mSurfaceComponent->deleteLater();
    if (surfaceComponent != nullptr)
        surfaceComponent->setParent(this);

    this->mSurfaceComponent = surfaceComponent;
    Q_EMIT surfaceComponentChanged();

    // NOTE: init start
    initScreens();
    ExtSessionLockV1Qt::Command::instance()->LockScreen();
    m_lock = true;
    Q_EMIT lockStateChanged();
}

void
WlSessionLock::onScreenAdded(QScreen *screen)
{
    if (!m_surfaces.contains(screen)) {
        auto *instanceObj =
          this->mSurfaceComponent->create(QQmlEngine::contextForObject(this->mSurfaceComponent));
        auto *instance = qobject_cast<WlSessionLockSurface *>(instanceObj);

        if (instance == nullptr) {
            qWarning()
              << "WlSessionLock.surface does not create a WlSessionLockSurface. Aborting lock.";
            if (instanceObj != nullptr)
                instanceObj->deleteLater();
            return;
        }

        instance->setParent(this);
        instance->setScreen(screen);

        m_surfaces[screen] = instance;
    }
}
void
WlSessionLock::onScreenRemoved(QScreen *screen)
{
    if (m_surfaces.contains(screen)) {
        auto surface = m_surfaces[screen];
        m_surfaces.remove(screen);
        surface->deleteLater();
    }
}

void
WlSessionLock::setLocked(bool locked)
{
    if (locked) {
        ExtSessionLockV1Qt::Command::instance()->LockScreen();
    } else {
        ExtSessionLockV1Qt::Command::instance()->unLockScreen();
    }
    m_lock = locked;
    Q_EMIT lockStateChanged();
}

WlSessionLockSurface::WlSessionLockSurface(QObject *parent)
  : QObject(parent)
  , m_contentItem(new QQuickItem())
  , m_window(new QQuickWindow)
{
    QQmlEngine::setObjectOwnership(this->m_contentItem, QQmlEngine::CppOwnership);
    m_contentItem->setParent(this);
    m_contentItem->setParentItem(this->m_window->contentItem());
    connect(this->m_window, &QQuickWindow::widthChanged, this, [this](int width) {
        m_contentItem->setWidth(width);
    });
    connect(this->m_window, &QQuickWindow::heightChanged, this, [this](int width) {
        m_contentItem->setHeight(width);
    });
}

void
WlSessionLockSurface::setScreen(QScreen *screen)
{
    ExtSessionLockV1Qt::Window::registerWindowFromQtScreen(m_window, screen);
    auto input = m_window->findChild<QQuickItem *>("input");
    QObject::connect(input, &QQuickItem::focusChanged, input, [input](auto focus) {
        if (!focus)
            return;
        auto focusWindow  = input->window();
        auto wFocusWindow = dynamic_cast<QtWaylandClient::QWaylandWindow *>(focusWindow->handle());
        wFocusWindow->display()->handleWindowActivated(wFocusWindow);
        if (wFocusWindow->display()->defaultInputDevice() &&
            wFocusWindow->display()->defaultInputDevice()->keyboard()) {
            wFocusWindow->display()->defaultInputDevice()->keyboard()->mFocus =
              wFocusWindow->waylandSurface();
        }
        if (oldWindow && oldWindow != wFocusWindow) {
            oldWindow->display()->handleWindowDeactivated(oldWindow);
        }
        oldWindow = wFocusWindow;
    });

    m_window->show();
}

QQmlListProperty<QObject>
WlSessionLockSurface::data()
{
    return this->m_contentItem->property("data").value<QQmlListProperty<QObject>>();
}

QQuickItem *
WlSessionLockSurface::contentItem() const
{
    return this->m_contentItem;
}

WlSessionLockSurface::~WlSessionLockSurface()
{
    if (this->m_window != nullptr) {
        this->m_window->deleteLater();
    }
}
