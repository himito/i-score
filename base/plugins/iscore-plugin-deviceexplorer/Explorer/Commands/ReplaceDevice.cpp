#include <QAbstractItemModel>
#include <QString>
#include <algorithm>
#include <vector>

#include "ReplaceDevice.hpp"
#include <Device/Node/DeviceNode.hpp>
#include <Device/Protocol/DeviceSettings.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Explorer/Explorer/DeviceExplorerModel.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/model/path/Path.hpp>
#include <iscore/model/path/PathSerialization.hpp>
#include <iscore/model/tree/TreeNode.hpp>

namespace Explorer
{
namespace Command
{
// TODO fix this to use NodeUpdateProxy. Maybe it should be a Remove() followed
// by
// a LoadDevice() ?
ReplaceDevice::ReplaceDevice(
    Path<DeviceDocumentPlugin>&& device_tree,
    int deviceIndex,
    Device::Node&& rootNode)
    : m_deviceTree{device_tree}
    , m_deviceIndex(deviceIndex)
    , m_deviceNode{std::move(rootNode)}
{
  auto& explorer = m_deviceTree.find().explorer();
  m_savedNode = explorer.nodeFromModelIndex(
      explorer.index(m_deviceIndex, 0, QModelIndex()));
}

ReplaceDevice::ReplaceDevice(
    Path<DeviceDocumentPlugin>&& device_tree,
    int deviceIndex,
    Device::Node&& oldRootNode,
    Device::Node&& newRootNode)
    : m_deviceTree{device_tree}
    , m_deviceIndex(deviceIndex)
    , m_deviceNode{std::move(newRootNode)}
    , m_savedNode{std::move(oldRootNode)}
{
}

void ReplaceDevice::undo() const
{
  auto& explorer = m_deviceTree.find().explorer();

  explorer.removeRow(m_deviceIndex);
  explorer.addDevice(m_savedNode);
}

void ReplaceDevice::redo() const
{
  auto& explorer = m_deviceTree.find().explorer();

  const auto& cld = explorer.rootNode().children();
  for (auto it = cld.begin(); it != cld.end(); ++it)
  {
    auto ds = it->get<Device::DeviceSettings>();
    if (ds.name == m_deviceNode.get<Device::DeviceSettings>().name)
    {
      explorer.removeNode(it);
      break;
    }
  }

  explorer.addDevice(m_deviceNode);
}

void ReplaceDevice::serializeImpl(DataStreamInput& d) const
{
  d << m_deviceTree << m_deviceIndex << m_deviceNode << m_savedNode;
}

void ReplaceDevice::deserializeImpl(DataStreamOutput& d)
{
  d >> m_deviceTree >> m_deviceIndex >> m_deviceNode >> m_savedNode;
}
}
}
