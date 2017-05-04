#pragma once
#include <Process/State/MessageNode.hpp>
#include <QAbstractItemModel>
#include <QStringList>
#include <QVariant>
#include <iscore/model/tree/TreeNodeItemModel.hpp>
#include <iscore_plugin_scenario_export.h>
#include <qnamespace.h>

class QMimeData;
class QObject;
namespace iscore
{
class CommandStackFacade;
} // namespace iscore
namespace State
{
struct Message;
}

namespace Scenario
{
class StateModel;
/**
 * @brief The MessageItemModel class
 *
 * Used as a wrapper with trees of node_type, to represent them
 * the Qt way.
 *
 */
class ISCORE_PLUGIN_SCENARIO_EXPORT MessageItemModel final
    : public TreeNodeBasedItemModel<Process::MessageNode>
{
  Q_OBJECT

public:
  using node_type = TreeNodeBasedItemModel<Process::MessageNode>::node_type;

  enum class Column : int
  {
    Name = 0,
    Value,
    Count
  };

  MessageItemModel(
      const iscore::CommandStackFacade& stack,
      const StateModel&,
      QObject* parent);
  MessageItemModel& operator=(const MessageItemModel&);
  MessageItemModel& operator=(const node_type&);
  MessageItemModel& operator=(node_type&&);

  const Process::MessageNode& rootNode() const override
  {
    return m_rootNode;
  }

  Process::MessageNode& rootNode() override
  {
    return m_rootNode;
  }

  // AbstractItemModel interface
  int columnCount(const QModelIndex& parent) const override;

  QVariant data(const QModelIndex& index, int role) const override;
  bool
  setData(const QModelIndex& index, const QVariant& value, int role) override;

  QVariant headerData(
      int section, Qt::Orientation orientation, int role) const override;
  bool setHeaderData(
      int section,
      Qt::Orientation orientation,
      const QVariant& value,
      int role) override;

  QStringList mimeTypes() const override;
  QMimeData* mimeData(const QModelIndexList& indexes) const override;
  bool canDropMimeData(
      const QMimeData* data,
      Qt::DropAction action,
      int row,
      int column,
      const QModelIndex& parent) const override;
  bool dropMimeData(
      const QMimeData* data,
      Qt::DropAction action,
      int row,
      int column,
      const QModelIndex& parent) override;

  Qt::DropActions supportedDragActions() const override;
  Qt::DropActions supportedDropActions() const override;

  Qt::ItemFlags flags(const QModelIndex& index) const override;

  const StateModel& stateModel;

signals:
  void userMessage(const State::Message&);

private:
  node_type m_rootNode;

  const iscore::CommandStackFacade& m_stack;
};
}

DEFAULT_MODEL_METADATA(Scenario::MessageItemModel, "Message item model")
