#pragma once
#include <Device/Address/AddressSettings.hpp>
#include <Device/Node/DeviceNode.hpp>
#include <Explorer/Commands/DeviceExplorerCommandFactory.hpp>
#include <iscore/command/Command.hpp>
#include <iscore/model/path/Path.hpp>

struct DataStreamInput;
struct DataStreamOutput;

namespace Explorer
{
class DeviceDocumentPlugin;
namespace Command
{
class UpdateAddressSettings final : public iscore::Command
{
  ISCORE_COMMAND_DECL(
      DeviceExplorerCommandFactoryName(),
      UpdateAddressSettings,
      "Update an address")
public:
  UpdateAddressSettings(
      Path<DeviceDocumentPlugin>&& device_tree,
      const Device::NodePath& node,
      const Device::AddressSettings& parameters);

  void undo() const override;
  void redo() const override;

protected:
  void serializeImpl(DataStreamInput&) const override;
  void deserializeImpl(DataStreamOutput&) override;

private:
  Path<DeviceDocumentPlugin> m_devicesModel;

  Device::NodePath m_node;

  Device::AddressSettings m_oldParameters;
  Device::AddressSettings m_newParameters;
};
}
}
