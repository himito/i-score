#pragma once
#include <QAbstractItemModel>
#include <iscore/tools/Todo.hpp>
/**
 * @brief Base implementation of a tree for QAbstractItemModel
 *
 * Provides basic tree-like functionality
 * shared between item models that uses the NodeType.
 */
// TESTME
template <typename NodeType>
class TreeNodeBasedItemModel : public QAbstractItemModel
{
public:
  using node_type = NodeType;

  using QAbstractItemModel::QAbstractItemModel;
  virtual ~TreeNodeBasedItemModel() = default;
  virtual NodeType& rootNode() = 0;
  virtual const NodeType& rootNode() const = 0;

  NodeType& nodeFromModelIndex(const QModelIndex& index) const
  {
    auto n = index.isValid() ? static_cast<NodeType*>(index.internalPointer())
                             : const_cast<NodeType*>(&rootNode());

    ISCORE_ASSERT(n);
    return *n;
  }

  QModelIndex parent(const QModelIndex& index) const final override
  {
    if (!index.isValid())
      return QModelIndex();
    if (index.model() != this)
      return QModelIndex();

    const auto& node = nodeFromModelIndex(index);
    auto parentNode = node.parent();

    if (!parentNode)
      return QModelIndex();

    auto grandparentNode = parentNode->parent();

    if (!grandparentNode)
      return QModelIndex();

    const int rowParent = grandparentNode->indexOfChild(parentNode);
    if (rowParent == -1)
      return QModelIndex();

    return createIndex(rowParent, 0, parentNode);
  }

  QModelIndex
  index(int row, int column, const QModelIndex& parent) const final override
  {
    if (!hasIndex(row, column, parent))
      return QModelIndex();

    auto& parentItem = nodeFromModelIndex(parent);

    if (parentItem.hasChild(row))
      return createIndex(row, column, &parentItem.childAt(row));
    else
      return QModelIndex();
  }

  int rowCount(const QModelIndex& parent) const final override
  {
    if (parent.column() > 0)
      return 0;

    const auto& parentNode = nodeFromModelIndex(parent);
    return parentNode.childCount();
  }

  bool hasChildren(const QModelIndex& parent) const final override
  {
    const auto& parentNode = nodeFromModelIndex(parent);
    return parentNode.childCount() > 0;
  }
};
