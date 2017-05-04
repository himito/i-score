#include <ossia/editor/dataspace/dataspace_visitors.hpp> // REMOVEME
#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/expression/expression_atom.hpp>
#include <ossia/editor/expression/expression_composition.hpp>
#include <ossia/editor/expression/expression_not.hpp>
#include <ossia/editor/expression/expression_pulse.hpp>
#include <Process/State/MessageNode.hpp>
#include <boost/concept/usage.hpp>

#include <QByteArray>
#include <QChar>
#include <QDebug>
#include <QList>
#include <QMap>
#include <QString>
#include <algorithm>
#include <boost/range/algorithm/find_if.hpp>
#include <eggs/variant/variant.hpp>
#include <exception>
#include <iscore/tools/std/Optional.hpp>
#include <string>
#include <vector>

#include <ossia/detail/algorithms.hpp>
#include <ossia/editor/state/message.hpp>
#include <ossia/editor/state/state.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/domain/domain.hpp>
#include <Device/Address/AddressSettings.hpp>
#include <Device/Address/IOType.hpp>
#include <Device/Protocol/DeviceInterface.hpp>
#include <Device/Protocol/DeviceList.hpp>
#include <Engine/Executor/ExecutorContext.hpp>
#include <Engine/Executor/ProcessComponent.hpp>
#include <Engine/Executor/StateProcessComponent.hpp>
#include <Engine/LocalTree/LocalTreeDocumentPlugin.hpp>
#include <Engine/OSSIA2iscore.hpp>
#include <Engine/Protocols/OSSIADevice.hpp>
#include <Engine/iscore2OSSIA.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Scenario/Document/State/ItemModel/MessageItemModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <State/Address.hpp>
#include <State/Expression.hpp>
#include <State/Message.hpp>
#include <State/Relation.hpp>
#include <ossia/detail/apply.hpp>
#include <iscore/model/tree/InvisibleRootNode.hpp>

#include <boost/call_traits.hpp>
class NodeNotFoundException : public std::runtime_error
{
public:
  NodeNotFoundException(const State::Address& n)
    : std::runtime_error{"Address: " + n.toString().toStdString()
                         + "not found in actual tree."}
  {
  }
};

namespace Engine
{
namespace iscore_to_ossia
{

ossia::net::node_base*
createNodeFromPath(const QStringList& path, ossia::net::device_base& dev)
{
  using namespace ossia;
  // Find the relevant node to add in the device
  ossia::net::node_base* node = &dev.get_root_node();
  for (int i = 0; i < path.size(); i++)
  {
    auto cld = node->find_child(path[i].toStdString());
    if (!cld)
    {
      // We have to start adding sub-nodes from here.
      ossia::net::node_base* parentnode = node;
      for (int k = i; k < path.size(); k++)
      {
        auto newNode = parentnode->create_child(path[k].toStdString());
        if (path[k].toStdString() != newNode->get_name())
        {
          qDebug() << path[k] << newNode->get_name().c_str();
          for (const auto& node : parentnode->children())
          {
            qDebug() << node->get_name().c_str();
          }
          ISCORE_ABORT;
        }
        if (k == path.size() - 1)
        {
          node = newNode;
        }
        else
        {
          parentnode = newNode;
        }
      }

      break;
    }
    else
    {
      node = cld;
    }
  }

  return node;
}

ossia::net::node_base*
findNodeFromPath(const QStringList& path, ossia::net::device_base& dev)
{
  using namespace ossia;
  // Find the relevant node to add in the device
  ossia::net::node_base* node = &dev.get_root_node();
  for (int i = 0; i < path.size(); i++)
  {
    auto cld = node->find_child(path[i].toStdString());
    if (cld)
      node = cld;
    else
    {
      qDebug() << "looking for" << path
               << " -- last found: " << node->get_name() << "\n";
      return {};
    }
  }

  return node;
}

ossia::net::node_base*
getNodeFromPath(const QStringList& path, ossia::net::device_base& dev)
{
  using namespace ossia;
  // Find the relevant node to add in the device
  ossia::net::node_base* node = &dev.get_root_node();
  const int n = path.size();
  for (int i = 0; i < n ; i++)
  {
    auto cld = node->find_child(path[i].toStdString());

    ISCORE_ASSERT(cld);

    node = cld;
  }

  ISCORE_ASSERT(node);
  return node;
}

struct ossia_type_visitor
{
public:
  using return_type = ossia::val_type;
  return_type operator()() const { ISCORE_ABORT; }
  return_type operator()(const State::impulse&) const { return ossia::val_type::IMPULSE; }
  return_type operator()(int) const { return ossia::val_type::INT; }
  return_type operator()(float) const { return ossia::val_type::FLOAT; }
  return_type operator()(bool) const { return ossia::val_type::BOOL; }
  return_type operator()(const std::string&) const { return ossia::val_type::STRING; }
  return_type operator()(char) const { return ossia::val_type::CHAR; }
  return_type operator()(const State::vec2f&) const { return ossia::val_type::VEC2F; }
  return_type operator()(const State::vec3f&) const { return ossia::val_type::VEC3F; }
  return_type operator()(const State::vec4f&) const { return ossia::val_type::VEC4F; }
  return_type operator()(const State::tuple_t&) const { return ossia::val_type::TUPLE; }
};

void updateOSSIAAddress(
    const Device::FullAddressSettings& settings,
    ossia::net::address_base& addr)
{
  ISCORE_ASSERT(settings.ioType);
  addr.set_access(*settings.ioType);

  addr.set_bounding(settings.clipMode);

  addr.set_repetition_filter(
        ossia::repetition_filter(settings.repetitionFilter));

  addr.set_value_type(ossia::apply(ossia_type_visitor{}, settings.value.val.impl()));

  addr.set_value(Engine::iscore_to_ossia::toOSSIAValue(settings.value));

  addr.set_domain(settings.domain);

  addr.set_unit(settings.unit);

  addr.getNode().set_extended_attributes(settings.extendedAttributes);
}

void createOSSIAAddress(
    const Device::FullAddressSettings& settings, ossia::net::node_base& node)
{
  if (!settings.value.val.impl())
    return;

  auto addr = node.create_address(
        ossia::apply(ossia_type_visitor{}, settings.value.val.impl()));
  if (addr)
    updateOSSIAAddress(settings, *addr);
}

void updateOSSIAValue(const State::ValueImpl& iscore_data, ossia::value& val)
{
  struct
  {
    const State::ValueImpl& data;
    void operator()(ossia::impulse) const
    {
    }
    void operator()(int32_t& v) const
    {
      v = data.get<int>();
    }
    void operator()(float& v) const
    {
      v = data.get<float>();
    }
    void operator()(bool& v) const
    {
      v = data.get<bool>();
    }
    void operator()(char& v) const
    {
      v = data.get<char>();
    }
    void operator()(std::string& v) const
    {
      v = data.get<std::string>();
    }
    void operator()(ossia::vec2f& v) const
    {
      v = data.get<State::vec2f>();
    }
    void operator()(ossia::vec3f& v) const
    {
      v = data.get<State::vec3f>();
    }
    void operator()(ossia::vec4f& v) const
    {
      v = data.get<State::vec4f>();
    }
    void operator()(std::vector<ossia::value>& vec) const
    {
      const State::tuple_t& tuple = *data.target<State::tuple_t>();
      ISCORE_ASSERT(tuple.size() == vec.size());
      const int n = vec.size();
      for (int i = 0; i < n; i++)
      {
        updateOSSIAValue(tuple[i], vec[i]);
      }
    }
  } visitor{iscore_data};

  return ossia::apply_nonnull(visitor, val.v);
}

ossia::value toOSSIAValue(const State::Value& value)
{
  return State::toOSSIAValue(value.val);
}

ossia::net::address_base* address(
    const State::Address& addr,
    const Device::DeviceList& deviceList)
{
  auto dev_p = deviceList.findDevice(addr.device);
  if (dev_p)
  {
    // OPTIMIZEME by sorting by device prior
    // to this.
    const auto& dev = *dev_p;
    if (dev.connected())
    {
      if (auto casted_dev
          = dynamic_cast<const Engine::Network::OSSIADevice*>(&dev))
      {
        auto ossia_dev = casted_dev->getDevice();
        if (ossia_dev)
        {
          auto ossia_node =
              Engine::iscore_to_ossia::findNodeFromPath(
                addr.path,
                *ossia_dev);

          if (ossia_node)
            return ossia_node->get_address();
        }
      }
    }
  }

  return nullptr;
}

optional<ossia::message>
message(const State::Message& mess, const Device::DeviceList& deviceList)
{
  if(auto ossia_addr = address(mess.address.address, deviceList))
  {
    auto val = Engine::iscore_to_ossia::toOSSIAValue(mess.value);
    if (val.valid())
      return ossia::message{
        {*ossia_addr, mess.address.qualifiers.get().accessors},
        std::move(val),
            mess.address.qualifiers.get().unit};
  }

  return {};
}

template <typename Fun>
static void visit_node(const Process::MessageNode& root, Fun f)
{
  f(root);

  for (const auto& child : root.children())
  {
    visit_node(child, f);
  }
}

ISCORE_PLUGIN_ENGINE_EXPORT void state(
    ossia::state& parent,
    const Scenario::StateModel& iscore_state,
    const Engine::Execution::Context& ctx)
{
  auto& elts = parent;

  // For all elements where IOType != Invalid,
  // we add the elements to the state.

  auto& dl = ctx.devices.list();
  visit_node(iscore_state.messages().rootNode(), [&](const auto& n) {
    const auto& val = n.value();
    if (val)
    {
      elts.add(message(State::Message{Process::address(n), *val}, dl));
    }
  });

  for (auto& proc : iscore_state.stateProcesses)
  {
    auto fac = ctx.stateProcesses.factory(proc);
    if (fac)
    {
      elts.add(fac->make(proc, ctx));
    }
  }
}

ISCORE_PLUGIN_ENGINE_EXPORT ossia::state state(
    const Scenario::StateModel& iscore_state,
    const Engine::Execution::Context& ctx)
{
  ossia::state s;
  Engine::iscore_to_ossia::state(s, iscore_state, ctx);
  return s;
}

static ossia::Destination expressionAddress(
    const State::Address& addr, const Device::DeviceList& devlist)
{
  auto dev_p = devlist.findDevice(addr.device);
  if (!dev_p)
    throw NodeNotFoundException(addr);

  auto& device = *dev_p;
  if (!device.connected())
  {
    throw NodeNotFoundException(addr);
  }

  if (auto casted_dev
      = dynamic_cast<const Engine::Network::OSSIADevice*>(&device))
  {
    auto dev = casted_dev->getDevice();
    if (!dev)
      throw NodeNotFoundException(addr);

    auto n = findNodeFromPath(addr.path, *dev);
    if (n)
    {
      auto ossia_addr = n->get_address();
      if (ossia_addr)
        return ossia::Destination(*ossia_addr);
      else
        throw NodeNotFoundException(addr);
    }
    else
    {
      throw NodeNotFoundException(addr);
    }
  }
  else
  {
    throw NodeNotFoundException(addr);
  }
}

static ossia::expressions::expression_atom::val_t expressionOperand(
    const State::RelationMember& relm, const Device::DeviceList& list)
{
  using namespace eggs::variants;

  const struct
  {
  public:
    const Device::DeviceList& devlist;
    using return_type = ossia::expressions::expression_atom::val_t;
    return_type operator()(const State::Address& addr) const
    {
      return expressionAddress(addr, devlist);
    }

    return_type operator()(const State::Value& val) const
    {
      return toOSSIAValue(val);
    }

    return_type operator()(const State::AddressAccessor& acc) const
    {
      auto dest = expressionAddress(acc.address, devlist);
      dest.index = acc.qualifiers.get().accessors;
      return dest;
    }
  } visitor{list};

  return eggs::variants::apply(visitor, relm);
}

// State::Relation -> OSSIA::ExpressionAtom
static ossia::expression_ptr
expressionAtom(const State::Relation& rel, const Device::DeviceList& dev)
{
  using namespace eggs::variants;

  return ossia::expressions::make_expression_atom(
        expressionOperand(rel.lhs, dev), rel.op,
        expressionOperand(rel.rhs, dev));
}

static ossia::expression_ptr
expressionPulse(const State::Pulse& rel, const Device::DeviceList& dev)
{
  using namespace eggs::variants;

  return ossia::expressions::make_expression_pulse(
        expressionAddress(rel.address, dev));
}

ossia::expression_ptr
expression(const State::Expression& e, const Device::DeviceList& list)
{
  const struct
  {
    const State::Expression& expr;
    const Device::DeviceList& devlist;
    using return_type = ossia::expression_ptr;

    return_type operator()() const
    {
      return ossia::expressions::make_expression_true();
    }

    return_type operator()(const State::Relation& rel) const
    {
      return expressionAtom(rel, devlist);
    }
    return_type operator()(const State::Pulse& rel) const
    {
      return expressionPulse(rel, devlist);
    }

    return_type operator()(const State::BinaryOperator rel) const
    {
      const auto& lhs = expr.childAt(0);
      const auto& rhs = expr.childAt(1);
      return ossia::expressions::make_expression_composition(
            expression(lhs, devlist),
            rel,
            expression(rhs, devlist));
    }
    return_type operator()(const State::UnaryOperator) const
    {
      return ossia::expressions::make_expression_not(
            expression(expr.childAt(0), devlist));
    }
    return_type operator()(const InvisibleRootNode) const
    {
      if (expr.childCount() == 0)
      {
        // By default no expression == true
        return ossia::expressions::make_expression_true();
      }
      else if (expr.childCount() == 1)
      {
        return expression(expr.childAt(0), devlist);
      }
      else
      {
        ISCORE_ABORT;
      }
    }

  } visitor{e, list};

  return ossia::apply(visitor, e.impl());
}

ossia::net::address_base*
findAddress(const Device::DeviceList& devs, const State::Address& addr)
{
  auto dev_p = dynamic_cast<Engine::Network::OSSIADevice*>(
        devs.findDevice(addr.device));
  if (dev_p)
  {
    auto ossia_dev = dev_p->getDevice();
    if (ossia_dev)
    {
      auto node
          = Engine::iscore_to_ossia::findNodeFromPath(addr.path, *ossia_dev);
      if (node)
        return node->get_address();
    }
  }
  return {};
}

optional<ossia::Destination> makeDestination(
    const Device::DeviceList& devices, const State::AddressAccessor& addr)
{
  auto ossia_addr
      = Engine::iscore_to_ossia::findAddress(devices, addr.address);

  if (ossia_addr)
  {
    auto& qual = addr.qualifiers.get();
    return ossia::Destination{*ossia_addr, qual.accessors, qual.unit};
  }

  return {};
}
}
}
