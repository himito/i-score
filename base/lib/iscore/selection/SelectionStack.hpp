#pragma once
#include <QObject>
#include <QStack>
#include <iscore/selection/Selection.hpp>

class IdentifiedObjectAbstract;

namespace iscore
{
/**
 * @brief The SelectionStack class
 *
 * A stack of selected elements.
 * Each time a selection of objects is done in the software,
 * it should be added to this stack using SelectionDispatcher.
 * This way, the user will be able to browse through his previous selections.
 */
class ISCORE_LIB_BASE_EXPORT SelectionStack final : public QObject
{
  Q_OBJECT
public:
  SelectionStack();

  bool canUnselect() const;
  bool canReselect() const;
  void clear();
  void clearAllButLast();

  // Go to the previous set of selections
  void unselect();

  // Go to the next set of selections
  void reselect();

  // Push a new set of empty selection.
  void deselect();

  Selection currentSelection() const;

signals:
  void pushNewSelection(const Selection& s);
  void currentSelectionChanged(const Selection&);

private slots:
  void prune(IdentifiedObjectAbstract* p);

private:
  // Select new objects
  void push(const Selection& s);

  // m_unselectable always contains the empty set at the beginning
  QStack<Selection> m_unselectable;
  QStack<Selection> m_reselectable;
};
}
