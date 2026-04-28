#include "SessionLock.h"
#include "command.h"
#include "window.h"
#include <QDebug>
#include <private/qwaylandscreen_p.h>
#include <private/qwaylandsurface_p.h>
#include <private/qwaylandwindow_p.h>
#include <qloggingcategory.h>

Q_LOGGING_CATEGORY(SessionLockQt, "SessionLockQt");

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
                qCWarning(SessionLockQt)
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
            qCWarning(SessionLockQt)
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
    connect(this->m_window, &QQuickWindow::heightChanged, this, [this](int height) {
        m_contentItem->setHeight(height);
    });
}

void
WlSessionLockSurface::setScreen(QScreen *screen)
{
    ExtSessionLockV1Qt::Window::registerWindowFromQtScreen(m_window, screen);
    m_window->show();
    ExtSessionLockV1Qt::Command::instance()->LockScreen();
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
