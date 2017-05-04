#include "PanelDelegate.hpp"
#include <Process/LayerModelPanelProxy.hpp>
#include <Process/ProcessList.hpp>
#include <Process/Tools/ProcessPanelGraphicsProxy.hpp>
#include <QVBoxLayout>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentPresenter.hpp>
#include <core/document/DocumentModel.hpp>
#include <core/document/DocumentPresenter.hpp>
#include <iscore/application/GUIApplicationContext.hpp>
#include <iscore/widgets/ClearLayout.hpp>

namespace Scenario
{
PanelDelegate::PanelDelegate(const iscore::GUIApplicationContext& ctx)
    : iscore::PanelDelegate{ctx}, m_widget{new QWidget}
{
  m_widget->setLayout(new QVBoxLayout);
}

QWidget* PanelDelegate::widget()
{
  return m_widget;
}

const iscore::PanelStatus& PanelDelegate::defaultPanelStatus() const
{
  static const iscore::PanelStatus status{false, Qt::BottomDockWidgetArea, 10,
                                          QObject::tr("Process"),
                                          QObject::tr("Ctrl+Shift+P")};

  return status;
}

void PanelDelegate::on_modelChanged(
    iscore::MaybeDocument oldm, iscore::MaybeDocument newm)
{
  if (oldm)
  {
    auto old_bem = iscore::IDocument::try_get<Scenario::ScenarioDocumentModel>(
        oldm->document);
    if (old_bem)
    {
      for (auto con : m_connections)
      {
        QObject::disconnect(con);
      }

      m_connections.clear();
    }
  }

  if (!newm)
  {
    cleanup();
    return;
  }

  auto bep = iscore::IDocument::try_get<Scenario::ScenarioDocumentPresenter>(
      newm->document);

  if (!bep)
    return;

  m_connections.push_back(con(
      bep->focusManager(), &Process::ProcessFocusManager::sig_focusedViewModel,
      this, &PanelDelegate::on_focusedViewModelChanged));

  m_connections.push_back(
      con(bep->focusManager(),
          &Process::ProcessFocusManager::sig_defocusedViewModel, this,
          [&] { on_focusedViewModelChanged(nullptr); }));

  on_focusedViewModelChanged(bep->focusManager().focusedModel());
}

void PanelDelegate::cleanup()
{
  m_layerModel = nullptr;
  delete m_proxy;
  m_proxy = nullptr;
}

void PanelDelegate::on_focusedViewModelChanged(
    const Process::ProcessModel* theLM)
{
  if (theLM && m_layerModel && theLM == m_layerModel)
  {
    // We don't want to switch if we click on the same layer
    return;
  }
  else if (theLM && isInFullView(*theLM))
  {
    // We don't want to switch if we click into the background of the scenario
    return;
  }
  /*
  else if(!theLM)
  {
      return ;
  }
  */
  else if (theLM != m_layerModel)
  {
    m_layerModel = theLM;
    delete m_proxy;
    m_proxy = nullptr;

    iscore::clearLayout(m_widget->layout());
    if (!m_layerModel)
      return;

    auto fact = context()
                    .interfaces<Process::LayerFactoryList>()
                    .findDefaultFactory(
                        m_layerModel->concreteKey());

    m_proxy = fact->makePanel(*m_layerModel, this);
    if (m_proxy)
      m_widget->layout()->addWidget(m_proxy->widget());
  }
}

void PanelDelegate::on_focusedViewModelRemoved(
    const Process::ProcessModel* theLM)
{
  ISCORE_TODO;
}
}
