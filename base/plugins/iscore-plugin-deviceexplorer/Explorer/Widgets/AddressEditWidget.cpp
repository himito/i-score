//#include "AddressEditWidget.hpp"

//#include <QBoxLayout>
//#include <QLineEdit>
//#include <State/Widgets/AddressLineEdit.hpp>
//#include <iscore/widgets/MarginLess.hpp>

//#include "DeviceCompleter.hpp"
//#include <State/Address.hpp>
//#include <State/MessageListSerialization.hpp>

//#include <Device/Node/DeviceNode.hpp>
//#include <Device/QMenuView/qmenuview.h>
//#include <Explorer/Explorer/DeviceExplorerModel.hpp>
//#include <QContextMenuEvent>

//namespace Explorer
//{
//AddressEditWidget::AddressEditWidget(
//    DeviceExplorerModel& model, QWidget* parent)
//    : QWidget{parent}, m_model{model}
//{
//  setAcceptDrops(true);
//  auto lay = new iscore::MarginLess<QHBoxLayout>{this};

//  m_lineEdit = new State::AddressLineEdit<AddressEditWidget>{this};

//  connect(m_lineEdit, &QLineEdit::editingFinished, [&]() {
//    m_address = State::Address::fromString(m_lineEdit->text());
//    emit addressChanged(m_address);
//  });

//  m_lineEdit->setContextMenuPolicy(Qt::CustomContextMenu);
//  connect(
//      m_lineEdit, &QLineEdit::customContextMenuRequested, this,
//      &AddressEditWidget::customContextMenuEvent);
//    // LineEdit completion
//  m_lineEdit->setCompleter(new DeviceCompleter{model, this});
//  lay->addWidget(m_lineEdit);
//}

//void AddressEditWidget::setAddress(const State::Address& addr)
//{
//  m_address = addr;
//  m_lineEdit->setText(m_address.toString());
//}

//void AddressEditWidget::setAddressString(QString s)
//{
//  m_lineEdit->setText(std::move(s));
//  State::Address addr;
//  m_address = addr.fromString(s);
//}

//void AddressEditWidget::dragEnterEvent(QDragEnterEvent* event)
//{
//  const auto& formats = event->mimeData()->formats();
//  if (formats.contains(iscore::mime::messagelist()))
//  {
//    event->accept();
//  }
//}

//void AddressEditWidget::customContextMenuEvent(const QPoint& p)
//{
//  auto device_menu = new QMenuView{m_lineEdit};
//  device_menu->setModel(&m_model);
//  connect(device_menu, &QMenuView::triggered, this, [&](const QModelIndex& m) {
//    auto addr = Device::address(m_model.nodeFromModelIndex(m)).address;
//    setAddress(addr);
//    emit addressChanged(addr);
//  });

//  device_menu->exec(m_lineEdit->mapToGlobal(p));
//  delete device_menu;
//}

//void AddressEditWidget::dropEvent(QDropEvent* ev)
//{
//  auto mime = ev->mimeData();

//  if (mime->formats().contains(iscore::mime::messagelist()))
//  {
//    Mime<State::MessageList>::Deserializer des{*mime};
//    State::MessageList ml = des.deserialize();
//    if (!ml.empty())
//    {
//      setAddress(ml[0].address.address);
//      emit addressChanged(ml[0].address.address);
//    }
//  }
//}
//}
