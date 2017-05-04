import QtQuick 2.0

DynamicLineEditForm
{
    signal addressChanged(string addr)

    signal textChange(string txt)
    textField.onEditingFinished: { textChange(textField.text); }
    id: widg
    property alias dropper: dropper
    AddressDrop
    {
        id: dropper
        item: widg
    }
}
