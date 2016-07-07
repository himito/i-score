#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Constraint/ViewModels/Temporal/TemporalConstraintView.hpp>
#include <Scenario/Document/Constraint/ViewModels/Temporal/TemporalConstraintViewModel.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <iscore/document/DocumentInterface.hpp>
#include <QGraphicsScene>
#include <QList>

#include <Process/ModelMetadata.hpp>
#include <Scenario/Document/Constraint/ViewModels/ConstraintHeader.hpp>
#include <Scenario/Document/Constraint/ViewModels/ConstraintPresenter.hpp>
#include <Scenario/Document/Constraint/ViewModels/ConstraintViewModel.hpp>
#include "TemporalConstraintHeader.hpp"
#include "TemporalConstraintPresenter.hpp"
#include "TemporalConstraintView.hpp"
#include <iscore/selection/Selectable.hpp>
#include <iscore/tools/Todo.hpp>
#include <Scenario/Application/Drops/ScenarioDropHandler.hpp>
#include <Process/ProcessContext.hpp>
class QColor;
class QObject;
class QString;

namespace Scenario
{
TemporalConstraintPresenter::TemporalConstraintPresenter(
        const TemporalConstraintViewModel& cstr_model,
        const Process::ProcessPresenterContext& ctx,
        QGraphicsObject *parentobject,
        QObject* parent) :
    ConstraintPresenter {"TemporalConstraintPresenter",
                         cstr_model,
                         new TemporalConstraintView{*this, parentobject},
                         new TemporalConstraintHeader,
                         ctx,
                         parent}
{
    TemporalConstraintView& v = *Scenario::view(this);
    con(v, &TemporalConstraintView::constraintHoverEnter,
        this,       &TemporalConstraintPresenter::constraintHoverEnter);

    con(v, &TemporalConstraintView::constraintHoverLeave,
        this,       &TemporalConstraintPresenter::constraintHoverLeave);

    con(cstr_model.model(), &ConstraintModel::executionStateChanged,
        &v, &TemporalConstraintView::setExecutionState);
    auto& duration = cstr_model.model().duration;
    con(cstr_model.model().duration, &ConstraintDurations::playPercentageChanged,
        this, [&] (double percentage) {
        v.setExecutionDuration(duration.defaultDuration() * percentage);
    });
    const auto& metadata = m_viewModel.model().metadata;
    con(metadata, &ModelMetadata::labelChanged,
        &v, &TemporalConstraintView::setLabel);

    con(metadata,   &ModelMetadata::colorChanged,
        &v, [&] (ColorRef c) {
        v.setLabelColor(c);
        v.setColor(c);
    });

    con(metadata, &ModelMetadata::nameChanged,
        this, [&] (const QString& name) { m_header->setText(name); });


    v.setLabel(metadata.label());
    v.setLabelColor(metadata.color());
    v.setColor(metadata.color());
    m_header->setText(metadata.name());
    v.setExecutionState(m_viewModel.model().executionState());

    con(m_viewModel.model().selection, &Selectable::changed,
        &v, &TemporalConstraintView::setFocused);
    con(m_viewModel.model(), &ConstraintModel::focusChanged,
        &v, &TemporalConstraintView::setFocused);

    // Drop
    con(v, &TemporalConstraintView::dropReceived,
            this, [=] (
            const QPointF &pos,
            const QMimeData *mime) {
        m_context.app.components
               .factory<Scenario::ConstraintDropHandlerList>()
               .handle(m_viewModel.model(), mime);
    });

    // Change to full view when header is double-clicked
    connect(static_cast<TemporalConstraintHeader*>(m_header), &TemporalConstraintHeader::doubleClicked,
            this, [this] () {
        using namespace iscore::IDocument;
        auto& base = get<ScenarioDocumentModel> (*documentFromObject(m_viewModel.model()));

        base.setDisplayedConstraint(m_viewModel.model());
    });
}

TemporalConstraintPresenter::~TemporalConstraintPresenter()
{

    auto view = Scenario::view(this);
    // TODO deleteGraphicsObject
    if(view)
    {
        auto sc = view->scene();

        if(sc && sc->items().contains(view))
        {
            sc->removeItem(view);
        }

        view->deleteLater();
    }
}
}
