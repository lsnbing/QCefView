import QtQuick 2.12
import QtQuick.Controls 2.5
import QCefQMLView 1.0


Rectangle {
    id: page
    width: 800
    height: 500

    Rectangle{
        width: 10
        height: 10
        color: "red"
    }

    Text {
        id: name
        text: qsTr("text")
    }

    QCefQMLView {
        anchors.fill: parent
    }
}
