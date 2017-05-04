#include "DeviceInterface.hpp"
#include <Device/Address/AddressSettings.hpp>
#include <Device/Node/DeviceNode.hpp>

namespace Device
{
struct DeviceSettings;

DeviceInterface::DeviceInterface(Device::DeviceSettings s)
    : m_settings(std::move(s))
{
}

DeviceInterface::~DeviceInterface() = default;

const Device::DeviceSettings& DeviceInterface::settings() const
{
  return m_settings;
}

void DeviceInterface::addNode(const Device::Node& n)
{
  auto full = Device::FullAddressSettings::
      make<Device::FullAddressSettings::as_parent>(
          n.get<Device::AddressSettings>(), Device::address(*n.parent()));

  // Add in the device implementation
  addAddress(full);

  for (const auto& child : n)
  {
    addNode(child);
  }
}
}
