#pragma once
#include <State/Value.hpp>
#include <QDialog>
#include <iscore/widgets/WidgetWrapper.hpp>

class QWidget;
namespace State
{
struct Address;
struct Message;
class ValueWidget;
}

class QComboBox;
class QFormLayout;

namespace Explorer
{
class AddressEditWidget;
class DeviceExplorerModel;

/**
 * @brief The MessageEditDialog class
 *
 * A dialog used to edit a single message.
 * The edited address and value can be found in the respective methods
 * after edition, if the dialog was accepted.
 *
 * A device explorer model is used for completion of the address.
 */
class MessageEditDialog final : public QDialog
{
    public:
        MessageEditDialog(
                const State::Message& mess,
                DeviceExplorerModel* model,
                QWidget* parent);

        const State::Address& address() const;

        State::Value value() const;

    private:
        void initTypeCombo();
        void on_typeChanged(int t);

        const State::Message& m_message;

        AddressEditWidget* m_addr{};

        QFormLayout* m_lay{};
        QComboBox* m_typeCombo{};
        WidgetWrapper<State::ValueWidget>* m_val{};
};
}
