import QtQuick 2.0
import QtQuick.Controls 2.0

Item
{
    Text
    {
        x: 0
        y: 0
        width: 212
        height: 18
        text: qsTr("Switch")
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: 12
        textFormat: Text.PlainText
        verticalAlignment: Text.AlignBottom
    }

    Switch
    {
        x: 69
        y: 24
        enabled: false
    }
}
