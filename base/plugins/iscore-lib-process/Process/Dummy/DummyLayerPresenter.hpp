#pragma once
#include <Process/LayerPresenter.hpp>
#include <QPoint>

#include <Process/Focus/FocusDispatcher.hpp>
#include <Process/ZoomHelper.hpp>
#include <iscore/model/Identifier.hpp>
#include <iscore_lib_process_export.h>

namespace Process
{
class ProcessModel;
}
class QMenu;
class QObject;

namespace Dummy
{
class DummyLayerView;
class ISCORE_LIB_PROCESS_EXPORT DummyLayerPresenter final
    : public Process::LayerPresenter
{
public:
  explicit DummyLayerPresenter(
      const Process::ProcessModel& model,
      DummyLayerView* view,
      const Process::ProcessPresenterContext& ctx,
      QObject* parent);

  void setWidth(qreal width) override;
  void setHeight(qreal height) override;

  void putToFront() override;
  void putBehind() override;

  void on_zoomRatioChanged(ZoomRatio) override;

  void parentGeometryChanged() override;

  const Process::ProcessModel& model() const override;
  const Id<Process::ProcessModel>& modelId() const override;

private:
  const Process::ProcessModel& m_layer;
  DummyLayerView* m_view{};
};
}
