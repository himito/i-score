#pragma once
#include <Curve/CurveStyle.hpp>
#include <Curve/Process/CurveProcessPresenter.hpp>
#include <Interpolation/InterpolationProcess.hpp>
#include <Interpolation/InterpolationView.hpp>

namespace Interpolation
{
class Presenter : public Curve::CurveProcessPresenter<ProcessModel, View>
{
public:
  Presenter(
      const Curve::Style& style,
      const ProcessModel& layer,
      View* view,
      const Process::ProcessPresenterContext& context,
      QObject* parent)
      : CurveProcessPresenter{style, layer, view, context, parent}
  {
    con(m_layer, &ProcessModel::addressChanged, this,
        &Presenter::on_nameChanges);
    con(m_layer.metadata(), &iscore::ModelMetadata::NameChanged,
        this, &Presenter::on_nameChanges);

    m_view->setDisplayedName(m_layer.prettyName());
    m_view->showName(true);
  }

private:
  void on_nameChanges()
  {
    m_view->setDisplayedName(m_layer.prettyName());
  }
};
}
