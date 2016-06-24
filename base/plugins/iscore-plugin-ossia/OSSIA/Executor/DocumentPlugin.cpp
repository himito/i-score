#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>

#include "BaseScenarioElement.hpp"
#include "DocumentPlugin.hpp"
#include <iscore/plugins/documentdelegate/plugin/DocumentDelegatePluginModel.hpp>
#include <core/document/DocumentModel.hpp>
#include <core/document/Document.hpp>
#include <OSSIA/Executor/ConstraintElement.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>

#include <Scenario/Document/BaseScenario/BaseScenario.hpp>
class QObject;
namespace iscore {
class Document;
}  // namespace iscore

namespace RecreateOnPlay
{
DocumentPlugin::DocumentPlugin(
            iscore::Document& doc,
            QObject* parent):
    iscore::DocumentPlugin{doc.context(), "OSSIADocumentPlugin", parent},
    m_ctx{doc.context(),
          *this,
          doc.context().plugin<Explorer::DeviceDocumentPlugin>(),
          doc.context().app.components.factory<ProcessComponentFactoryList>(),
          doc.context().app.components.factory<StateProcessComponentFactoryList>(),
          }
{
    con(doc, &iscore::Document::aboutToClose,
        this, [&]
    {
        if(m_base)
        {
            m_base->baseConstraint()->stop();
        }
        m_base.reset();
    });
}

DocumentPlugin::~DocumentPlugin()
{
    if(m_base)
    {
        m_base->baseConstraint()->stop();
    }
}

void DocumentPlugin::reload(Scenario::ConstraintModel& cst)
{
    if(m_base)
    {
        m_base->baseConstraint()->stop();
    }
    auto parent = dynamic_cast<Scenario::ScenarioInterface*>(cst.parent());
    ISCORE_ASSERT(parent);
    m_base = std::make_unique<BaseScenarioElement>(BaseScenarioRefContainer{cst, *parent}, m_ctx, this);
}

void DocumentPlugin::clear()
{
    m_base.reset();
}

BaseScenarioElement *DocumentPlugin::baseScenario() const
{
    return m_base.get();
}

bool DocumentPlugin::isPlaying() const
{
    return m_base.get();
}
}
