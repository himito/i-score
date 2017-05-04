#pragma once
#include <Device/Address/AddressSettings.hpp>
#include <Device/Protocol/DeviceSettings.hpp>
#include <QList>
#include <QString>
#include <QStringList>
#include <State/Message.hpp>
#include <algorithm>
#include <iscore/model/tree/TreeNode.hpp>
#include <iscore/model/tree/TreePath.hpp>
#include <iscore/model/tree/VariantBasedNode.hpp>

#include <State/Address.hpp>
#include <iscore_lib_device_export.h>

class DataStream;
class JSONObject;

namespace Device
{
struct AddressSettings;
struct DeviceSettings;

class ISCORE_LIB_DEVICE_EXPORT DeviceExplorerNode
    : public iscore::
          VariantBasedNode<Device::DeviceSettings, Device::AddressSettings>
{
  ISCORE_SERIALIZE_FRIENDS

public:
  enum class Type
  {
    RootNode,
    Device,
    Address
  };

  DeviceExplorerNode(const DeviceExplorerNode& t) = default;
  DeviceExplorerNode(DeviceExplorerNode&& t) = default;
  DeviceExplorerNode& operator=(const DeviceExplorerNode& t) = default;
  DeviceExplorerNode() = default;
  template <typename T>
  DeviceExplorerNode(const T& t) : VariantBasedNode{t}
  {
  }
  template <typename T>
  DeviceExplorerNode(T&& t) : VariantBasedNode{std::move(t)}
  {
  }

  bool operator==(const DeviceExplorerNode& other) const
  {
    return static_cast<const VariantBasedNode&>(*this) == static_cast<const VariantBasedNode&>(other);
  }

  //- accessors
  QString displayName() const;

  bool isSelectable() const;
  bool isEditable() const;
};

using Node = TreeNode<DeviceExplorerNode>;
using NodePath = TreePath<Device::Node>;

// TODO reflist may be a better name.
using FreeNode = std::pair<State::Address, Device::Node>;
using NodeList = std::vector<Device::Node*>;
using FreeNodeList = std::vector<FreeNode>;

// TODO add specifications & tests to these functions

ISCORE_LIB_DEVICE_EXPORT QString deviceName(const Node& treeNode);
ISCORE_LIB_DEVICE_EXPORT State::AddressAccessor address(const Node& treeNode);

ISCORE_LIB_DEVICE_EXPORT State::Message message(const Device::Node& node);

/**
 * @brief parametersList Recursive list of parameters in this node
 *
 * Note : this one takes an output reference as an optimization
 * because of its use in DeviceExplorerModel::indexesToMime
 */
ISCORE_LIB_DEVICE_EXPORT void
parametersList(const Node& treeNode, State::MessageList& ml);

// TODO have all these guys return references
ISCORE_LIB_DEVICE_EXPORT Device::Node&
getNodeFromAddress(Device::Node& root, const State::Address&);
ISCORE_LIB_DEVICE_EXPORT Device::Node*
getNodeFromString(Device::Node& n, QStringList&& str); // Fails if not present.

/**
 * @brief dumpTree An utility to print trees
 * of Device::Nodes
 */
ISCORE_LIB_DEVICE_EXPORT void dumpTree(const Device::Node& node, QString rec);

ISCORE_LIB_DEVICE_EXPORT Device::Node
merge(Device::Node base, const State::MessageList& other);

ISCORE_LIB_DEVICE_EXPORT void
merge(Device::Node& base, const State::Message& message);

// True if gramps is a parent, grand-parent, etc. of node.
template <typename Node_T>
bool isAncestor(const Node_T& gramps, const Node_T* node)
{
  auto parent = node->parent();
  if (!parent)
    return false;

  if (node == &gramps)
    return true;

  return isAncestor(gramps, parent);
}

/**
 * @brief filterUniqueParents
 * @param nodes A list of nodes
 * @return Another list of nodes
 *
 * This function filters a list of node
 * by only keeping the nodes that had no ancestor.
 *
 * e.g. given the tree :
 *
 * a -> b -> d
 *        -> e
 *   -> c
 * f -> g
 *
 * If the input consists of b, d, the output will be b.
 * If the input consists of a, b, d, f, the output will be a, f.
 * If the input consists of d, e, the output will be d, e.
 *
 * TESTME
 */
template <typename Node_T>
std::vector<Node_T*> filterUniqueParents(std::vector<Node_T*>& nodes)
{
  std::vector<Node_T*> cleaned_nodes;

  ossia::sort(nodes);
  nodes.erase(ossia::unique(nodes), nodes.end());

  cleaned_nodes.reserve(nodes.size());

  // Only copy the index if it none of its parents
  // except the invisible root are in the list.
  for (auto n : nodes)
  {
    if (ossia::any_of(nodes, [&](Node_T* other) {
          if (other == n)
            return false;
          return isAncestor(*other, n);
        }))
    {
      continue;
    }
    else
    {
      cleaned_nodes.push_back(n);
    }
  }

  return cleaned_nodes;
}

// Generic algorithms for DeviceExplorerNode-like structures.
template <typename Node_T>
Node_T* try_getNodeFromString(Node_T& n, QStringList&& parts)
{
  if (parts.size() == 0)
    return &n;

  for (auto& child : n)
  {
    if (child.displayName() == parts[0])
    {
      parts.removeFirst();
      return try_getNodeFromString(child, std::move(parts));
    }
  }

  return nullptr;
}

template <typename Node_T>
Node_T* try_getNodeFromAddress(Node_T& root, const State::Address& addr)
{
  if (addr.device.isEmpty())
    return &root;

  auto dev = std::find_if(root.begin(), root.end(), [&](const Node_T& n) {
    return n.template is<Device::DeviceSettings>()
           && n.template get<Device::DeviceSettings>().name == addr.device;
  });

  if (dev == root.end())
    return nullptr;

  return try_getNodeFromString(*dev, QStringList(addr.path));
}
}
