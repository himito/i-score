#include "LocalTreeDocumentPlugin.hpp"
#include <ossia/editor/state/state_element.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/device.hpp>

#include <Automation/AutomationModel.hpp>
#include <Curve/CurveModel.hpp>

#include <Scenario/Document/BaseScenario/BaseScenario.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <Scenario/Document/State/ItemModel/MessageItemModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/TimeNode/TimeNodeModel.hpp>
#include <Scenario/Document/TimeNode/Trigger/TriggerModel.hpp>
#include <Scenario/Process/Algorithms/Accessors.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include <State/Message.hpp>
#include <State/Value.hpp>

#include <Process/State/MessageNode.hpp>

#include <Curve/Segment/CurveSegmentData.hpp>

#include <Engine/OSSIA2iscore.hpp>
#include <Engine/iscore2OSSIA.hpp>
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <iscore/document/DocumentInterface.hpp>

#include "Scenario/ScenarioComponent.hpp"
#include <Engine/LocalTree/Settings/LocalTreeModel.hpp>
#include <Engine/Protocols/Local/LocalProtocolFactory.hpp>
#include <Engine/Protocols/Local/LocalSpecificSettings.hpp>
#include <Engine/ApplicationPlugin.hpp>
#include <iscore/actions/ActionManager.hpp>
#include <Scenario/Application/ScenarioActions.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>

Engine::LocalTree::DocumentPlugin::DocumentPlugin(
    const iscore::DocumentContext& ctx, Id<iscore::DocumentPlugin> id, QObject* parent)
    : iscore::DocumentPlugin{ctx, std::move(id), "LocalTree::DocumentPlugin", parent}
    , m_localDevice{std::make_unique<ossia::net::multiplex_protocol>(), "i-score"}
    , m_localDeviceWrapper{
          m_localDevice, ctx,
          Network::LocalProtocolFactory::static_defaultSettings()}
{
}

Engine::LocalTree::DocumentPlugin::~DocumentPlugin()
{
  cleanup();

  auto docplug = context().findPlugin<Explorer::DeviceDocumentPlugin>();
  if (docplug)
    docplug->list().setLocalDevice(nullptr);
}

void Engine::LocalTree::DocumentPlugin::init()
{
  auto& set = m_context.app.settings<Settings::Model>();
  if (set.getLocalTree())
  {
    create();
  }

  con(set, &Settings::Model::LocalTreeChanged, this,
      [=](bool b) {
        if (b)
          create();
        else
          cleanup();
      },
      Qt::QueuedConnection);

  auto docplug = context().findPlugin<Explorer::DeviceDocumentPlugin>();
  if (docplug)
    docplug->list().setLocalDevice(&m_localDeviceWrapper);
}

void Engine::LocalTree::DocumentPlugin::on_documentClosing()
{
  cleanup();
}

void Engine::LocalTree::DocumentPlugin::create()
{
  if (m_root)
    cleanup();

  auto& doc = m_context.document.model().modelDelegate();
  auto scenar = dynamic_cast<Scenario::ScenarioDocumentModel*>(&doc);
  if (!scenar)
    return;

  auto& cstr = scenar->baseScenario().constraint();
  m_root = new Constraint(
      m_localDevice.get_root_node(),
      getStrongId(cstr.components()),
      cstr,
      *this,
      this);
  cstr.components().add(m_root);
  m_root->node().set_name("root");
}

void Engine::LocalTree::DocumentPlugin::cleanup()
{
  if (!m_root)
    return;

  m_root->constraint().components().remove(m_root);
  m_root = nullptr;
}
