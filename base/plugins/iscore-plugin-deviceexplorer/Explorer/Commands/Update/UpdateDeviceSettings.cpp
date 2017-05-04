#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <algorithm>
#include <vector>

#include "UpdateDeviceSettings.hpp"
#include <Device/Node/DeviceNode.hpp>
#include <Device/Protocol/DeviceSettings.hpp>
#include <Explorer/DocumentPlugin/NodeUpdateProxy.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/model/path/Path.hpp>
#include <iscore/model/path/PathSerialization.hpp>
#include <iscore/model/tree/TreeNode.hpp>

namespace Explorer
{
namespace Command
{
UpdateDeviceSettings::UpdateDeviceSettings(
    Path<DeviceDocumentPlugin>&& device_tree,
    const QString& name,
    const Device::DeviceSettings& parameters)
    : m_devicesModel{device_tree}, m_newParameters(parameters)
{
  auto& devplug = m_devicesModel.find();
  auto it = std::find_if(
      devplug.rootNode().begin(),
      devplug.rootNode().end(),
      [&](const Device::Node& n) {
        return n.get<Device::DeviceSettings>().name == name;
      });

  ISCORE_ASSERT(it != devplug.rootNode().end());

  m_oldParameters = (*it).get<Device::DeviceSettings>();
}

void UpdateDeviceSettings::undo() const
{
  auto& devplug = m_devicesModel.find();
  devplug.updateProxy.updateDevice(m_newParameters.name, m_oldParameters);
}

void UpdateDeviceSettings::redo() const
{
  auto& devplug = m_devicesModel.find();
  devplug.updateProxy.updateDevice(m_oldParameters.name, m_newParameters);
}

void UpdateDeviceSettings::serializeImpl(DataStreamInput& d) const
{
  d << m_devicesModel << m_oldParameters << m_newParameters;
}

void UpdateDeviceSettings::deserializeImpl(DataStreamOutput& d)
{
  d >> m_devicesModel >> m_oldParameters >> m_newParameters;
}
}
}
