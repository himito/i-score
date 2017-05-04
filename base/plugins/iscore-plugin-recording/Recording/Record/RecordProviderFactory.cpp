#include "RecordProviderFactory.hpp"
#include <Recording/Commands/Record.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <iscore/document/DocumentContext.hpp>
namespace Recording
{
RecordProvider::~RecordProvider() = default;
RecorderFactory::~RecorderFactory() = default;

RecordContext::RecordContext(
    const Scenario::ProcessModel& scenar, Scenario::Point pt)
    : context{iscore::IDocument::documentContext(scenar)}
    , scenario{scenar}
    , explorer{Explorer::deviceExplorerFromContext(context)}
    , dispatcher{context.commandStack}
    , point{pt}

{
  connect(this, &RecordContext::startTimer,
          this, &RecordContext::on_startTimer,
          Qt::QueuedConnection);
}
}
