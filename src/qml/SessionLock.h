#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QQuickItem>
#include <qquickwindow.h>

class WlSessionLockSurface;

class WlSessionLock : public QObject
{
    Q_OBJECT

    // clang-format off
    Q_PROPERTY(QQmlComponent* surface READ surfaceComponent WRITE setSurfaceComponent NOTIFY surfaceComponentChanged);

    // clang-format on
    QML_ELEMENT
    Q_CLASSINFO("DefaultProperty", "surface");

public:
    explicit WlSessionLock(QObject *parent = nullptr);

    Q_PROPERTY(bool locked READ isLocked WRITE setLocked NOTIFY lockStateChanged);
    inline bool isLocked() { return m_lock; }
    void setLocked(bool locked);

    [[nodiscard]] QQmlComponent *surfaceComponent() const;
    void setSurfaceComponent(QQmlComponent *surfaceComponent);

private slots:
    void onScreenAdded(QScreen *screen);
    void onScreenRemoved(QScreen *screen);
signals:
    void lockStateChanged();
    void surfaceComponentChanged();

private:
    void initScreens();

private:
    bool m_lock                      = false;
    QQmlComponent *mSurfaceComponent = nullptr;
    QMap<QScreen *, WlSessionLockSurface *> m_surfaces;
    friend class WlSessionLockSurface;
};

class WlSessionLockSurface : public QObject
{
    Q_OBJECT

    QML_NAMED_ELEMENT(WlSessionLockSurface);
    Q_CLASSINFO("DefaultProperty", "data");

public:
    explicit WlSessionLockSurface(QObject *parent = nullptr);
    ~WlSessionLockSurface() override;
    Q_DISABLE_COPY_MOVE(WlSessionLockSurface);

    Q_PROPERTY(QQuickItem *contentItem READ contentItem);

    [[nodiscard]] QQuickItem *contentItem() const;
    Q_PROPERTY(QQmlListProperty<QObject> data READ data);
    [[nodiscard]] QQmlListProperty<QObject> data();

    void setScreen(QScreen *qscreen);

private:
    QQuickWindow *m_window;
    QQuickItem *m_contentItem;
};
