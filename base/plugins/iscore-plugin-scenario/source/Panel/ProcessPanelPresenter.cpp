#include "ProcessPanelPresenter.hpp"
#include "ProcessPanelView.hpp"
#include "ProcessPanelModel.hpp"

#include <QApplication>
#include <Document/BaseElement/BaseElementModel.hpp>

#include <ProcessInterface/ProcessList.hpp>
#include <ProcessInterface/ProcessModel.hpp>
#include <ProcessInterface/ProcessViewModel.hpp>
#include <ProcessInterface/ProcessViewModelPanelProxy.hpp>
#include <ProcessInterface/ProcessPresenter.hpp>
#include <ProcessInterface/ProcessView.hpp>
#include <ProcessInterface/ProcessFactory.hpp>

#include <Document/BaseElement/Widgets/SizeNotifyingGraphicsView.hpp>

#include "ProcessPanelId.hpp"
#include "Document/BaseElement/ProcessFocusManager.hpp"
#include "Document/BaseElement/Widgets/DoubleSlider.hpp"

ProcessPanelPresenter::ProcessPanelPresenter(iscore::Presenter* parent_presenter, iscore::PanelView* view):
    iscore::PanelPresenter{parent_presenter, view}
{
    auto v = static_cast<ProcessPanelView*>(view);
    connect(v, &ProcessPanelView::horizontalZoomChanged,
            this, &ProcessPanelPresenter::on_zoomChanged);
}

int ProcessPanelPresenter::panelId() const
{
    return PROCESS_PANEL_ID;
}

void ProcessPanelPresenter::on_modelChanged()
{
    if(!model())
    {
        cleanup();
        return;
    }

    auto bem = iscore::IDocument::documentFromObject(model())
                    ->findChild<BaseElementModel*>("BaseElementModel");

    if(!bem)
        return;

    connect(&bem->focusManager(),  &ProcessFocusManager::sig_focusedViewModel,
            this, &ProcessPanelPresenter::on_focusedViewModelChanged);

    auto panelview = static_cast<ProcessPanelView*>(view());
    connect(panelview, &ProcessPanelView::sizeChanged,
            this, &ProcessPanelPresenter::on_sizeChanged);

    on_focusedViewModelChanged(bem->focusManager().focusedViewModel());
}

void ProcessPanelPresenter::on_focusedViewModelChanged(const ProcessViewModel* thePVM)
{
    if(thePVM != m_processViewModel)
    {
        m_processViewModel = thePVM;
        delete m_processPresenter;
        m_processPresenter = nullptr;

        if(!m_processViewModel)
            return;

        auto& sharedmodel = m_processViewModel->sharedProcessModel();
        auto fact = ProcessList::getFactory(sharedmodel.processName());

        auto proxy = m_processViewModel->make_panelProxy(this);

        delete m_obj;
        m_obj = new ProcessPanelGraphicsProxy{*thePVM, *this};
        // Add the items to the scene early because
        // the presenters might call scene() in their ctor.
        auto panelview = static_cast<ProcessPanelView*>(view());
        panelview->scene()->addItem(m_obj);

        m_processView = fact->makeView(proxy->viewModel(),
                                       m_obj);

        m_processPresenter = fact->makePresenter(proxy->viewModel(),
                                                 m_processView,
                                                 this);

        connect(m_processViewModel, &QObject::destroyed,
                this, &ProcessPanelPresenter::cleanup);


        // Have a zoom here too. For now the process should be the size of the window.
        on_sizeChanged(panelview->view()->size());

        on_zoomChanged(0.03);
    }
}

void ProcessPanelPresenter::on_sizeChanged(const QSize& size)
{
    if(!m_processPresenter)
        return;

    m_processPresenter->setHeight(size.height());
    m_processPresenter->parentGeometryChanged();

    auto fullWidth = m_processPresenter->viewModel().sharedProcessModel().duration().toPixels(m_zoomRatio);

    m_obj->setSize(QSizeF{(double)fullWidth, (double)size.height()});
}

void ProcessPanelPresenter::on_zoomChanged(ZoomRatio newzoom)
{
    // TODO refactor this with what's in base element model
    // mapZoom maps a value between 0 and 1 to the correct zoom.
    auto mapZoom = [] (double val, double min, double max)
    { return (max - min) * val + min; };

    // computedMax : the number of pixels in a millisecond when the whole constraint
    // is displayed on screen;

    // We want the value to be at least twice the duration of the constraint
    const auto& viewsize = static_cast<ProcessPanelView*>(view())->view()->size();
    const auto& duration =  m_processViewModel->sharedProcessModel().duration();

    m_zoomRatio = mapZoom(1.0 - newzoom,
                          2.,
                          std::max(4., 2 * duration.msec() / viewsize.width()));

    auto panelview = static_cast<ProcessPanelView*>(view());
    panelview->view()->setSceneRect(
                0, 0, duration.toPixels(m_zoomRatio) * 1.2,  viewsize.height());

    m_processPresenter->on_zoomRatioChanged(m_zoomRatio);

    auto fullWidth = duration.toPixels(m_zoomRatio);
    m_processPresenter->setWidth(fullWidth);

    m_processPresenter->parentGeometryChanged();
    m_obj->setSize(QSizeF{(double)fullWidth, (double)viewsize.height()});
}

void ProcessPanelPresenter::cleanup()
{
    m_processViewModel = nullptr;

    delete m_processPresenter; // Will delete the view, too
    m_processPresenter = nullptr;

    delete m_obj;
    m_obj = nullptr;
}