#include <QDebug>
#include <QString>
#include <QVariant>
#include <memory>

#include "OSCDevice.hpp"
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/osc/osc.hpp>
#include <Device/Protocol/DeviceSettings.hpp>
#include <Engine/Protocols/OSC/OSCSpecificSettings.hpp>

namespace Engine
{
namespace Network
{
OSCDevice::OSCDevice(const Device::DeviceSettings& settings)
    : OwningOSSIADevice{settings}
{
  using namespace ossia;
  m_capas.canLearn = true;
  m_capas.hasCallbacks = false;

  reconnect();
}

bool OSCDevice::reconnect()
{
  disconnect();

  try
  {
    auto stgs = settings().deviceSpecificSettings.value<OSCSpecificSettings>();
    std::unique_ptr<ossia::net::protocol_base> ossia_settings
        = std::make_unique<ossia::net::osc_protocol>(
            stgs.host.toStdString(), stgs.inputPort, stgs.outputPort);
    m_dev = std::make_unique<ossia::net::generic_device>(
        std::move(ossia_settings), settings().name.toStdString());
    setLogging_impl(isLogging());
  }
  catch (...)
  {
    ISCORE_TODO;
  }

  return connected();
}

void OSCDevice::recreate(const Device::Node& n)
{
  for(auto& child : n)
  {
    addNode(child);
  }
}

bool OSCDevice::isLearning() const
{
  auto& proto = static_cast<ossia::net::osc_protocol&>(m_dev->get_protocol());
  return proto.learning();
}

void OSCDevice::setLearning(bool b)
{
  auto& proto = static_cast<ossia::net::osc_protocol&>(m_dev->get_protocol());
  auto& dev = *m_dev;
  if (b)
  {
    dev.on_node_created.connect<OSSIADevice, &OSSIADevice::nodeCreated>(
        (OSSIADevice*)this);
    dev.on_node_removing.connect<OSSIADevice, &OSSIADevice::nodeRemoving>(
        (OSSIADevice*)this);
    dev.on_node_renamed.connect<OSSIADevice, &OSSIADevice::nodeRenamed>(
        (OSSIADevice*)this);
    dev.on_address_created.connect<OSSIADevice, &OSSIADevice::addressCreated>(
        (OSSIADevice*)this);
    dev.on_attribute_modified.connect<OSSIADevice, &OSSIADevice::addressUpdated>(
        (OSSIADevice*)this);
  }
  else
  {
    dev.on_node_created.disconnect<OSSIADevice, &OSSIADevice::nodeCreated>(
        (OSSIADevice*)this);
    dev.on_node_removing.disconnect<OSSIADevice, &OSSIADevice::nodeRemoving>(
        (OSSIADevice*)this);
    dev.on_node_renamed.disconnect<OSSIADevice, &OSSIADevice::nodeRenamed>(
        (OSSIADevice*)this);
    dev.on_address_created.disconnect<OSSIADevice, &OSSIADevice::addressCreated>(
        (OSSIADevice*)this);
    dev.on_attribute_modified
        .disconnect<OSSIADevice, &OSSIADevice::addressUpdated>(
            (OSSIADevice*)this);
  }

  proto.set_learning(b);
}
}
}
