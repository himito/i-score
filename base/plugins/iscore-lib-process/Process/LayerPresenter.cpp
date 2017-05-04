#include "LayerPresenter.hpp"
namespace Process
{
LayerPresenter::~LayerPresenter() = default;

bool LayerPresenter::focused() const
{
  return m_focus;
}

void LayerPresenter::setFocus(bool focus)
{
  m_focus = focus;
  on_focusChanged();
}

void LayerPresenter::on_focusChanged()
{
}

void LayerPresenter::fillContextMenu(
    QMenu&, QPoint pos, QPointF scenepos, const LayerContextMenuManager&) const
{
}
}
