pragma ComponentBehavior: Bound
import WayCrate.SessionLock
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

WlSessionLock {
    id: lockMain
    WlSessionLockSurface {
        ColumnLayout {
            anchors.fill: parent
            Button {
                text: "hello"
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                onClicked: {
                    lockMain.locked = false;
                    Qt.quit()
                }
            }
        }
    }
}
