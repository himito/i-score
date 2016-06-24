#include <Editor/TimeConstraint.h>
#include <Editor/StateElement.h>
#include <Network/Device.h>
#include <Network/Protocol/Local.h>
#include <Explorer/Explorer/DeviceExplorerModel.hpp>
#include <Network/Protocol/OSC.h>
#include <OSSIA/Executor/BaseScenarioElement.hpp>
#include <OSSIA/Executor/DocumentPlugin.hpp>
#include <Scenario/Application/ScenarioApplicationPlugin.hpp>
#include <QAction>
#include <QVariant>
#include <QVector>

#include "Editor/Value.h"

#include "Network/Address.h"
#include "Network/Node.h"
#include "OSSIAApplicationPlugin.hpp"
#include <Process/TimeValue.hpp>
#include <OSSIA/Executor/ConstraintElement.hpp>
#include <OSSIA/Executor/StateElement.hpp>
#include <Network/Protocol/Minuit.h>

#include <iscore/application/ApplicationContext.hpp>
#include <iscore/plugins/application/GUIApplicationContextPlugin.hpp>
#include <iscore/tools/Todo.hpp>
#include <Scenario/Application/ScenarioActions.hpp>

struct VisitorVariant;
#if defined(ISCORE_DEPLOYMENT_BUILD) && (defined(__APPLE__) || defined(linux))
#include <TTFoundationAPI.h>
#include <TTModular.h>
#include <QFileInfo>
#include <QDir>
#endif
#include <OSSIA/LocalTree/LocalTreeDocumentPlugin.hpp>
#include <OSSIA/Executor/ContextMenu/PlayContextMenu.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <core/document/Document.hpp>
#include <core/presenter/DocumentManager.hpp>
#include <core/document/DocumentModel.hpp>
#include <core/application/ApplicationSettings.hpp>
#include <OSSIA/Executor/ClockManager/ClockManagerFactory.hpp>
#include <algorithm>
#include <vector>

#include <OSSIA/Executor/Settings/ExecutorModel.hpp>

OSSIAApplicationPlugin::OSSIAApplicationPlugin(
        const iscore::GUIApplicationContext& ctx):
    iscore::GUIApplicationContextPlugin {ctx},
    m_playActions{*this, ctx}
{
#if defined(ISCORE_DEPLOYMENT_BUILD)
// Here we try to load the extensions first because of buggy behaviour in TTExtensionLoader and API.
#if defined(__APPLE__)
    auto contents = QFileInfo(qApp->applicationDirPath()).dir().path() + "/Frameworks/jamoma/extensions";
    TTFoundationInit(contents.toUtf8().constData(), false);
    TTModularInit(contents.toUtf8().constData(), false);
#elif defined(linux)
    auto contents = QFileInfo(qApp->applicationDirPath()).dir().path() + "/../lib/jamoma";
    TTFoundationInit(contents.toUtf8().constData(), false);
    TTModularInit(contents.toUtf8().constData(), false);
#endif
#endif
    auto localDevice = OSSIA::Local::create();
    m_localDevice = OSSIA::Device::create(localDevice, "i-score");

    setupOSSIACallbacks();

    // Two parts :
    // One that maintains the devices for each document
    // (and disconnects / reconnects them when the current document changes)
    // Also during execution, one shouldn't be able to switch document.

    // Another part that, at execution time, creates structures corresponding
    // to the Scenario plug-in with the OSSIA API.


    auto& play_action = ctx.actions.action<Actions::Play>();
    connect(play_action.action(), &QAction::triggered,
            this, [&] (bool b)
    {
        on_play(b);
    });

    auto& stop_action = ctx.actions.action<Actions::Stop>();
    connect(stop_action.action(), &QAction::triggered,
            this, &OSSIAApplicationPlugin::on_stop);

    auto& init_action = ctx.actions.action<Actions::Reinitialize>();
    connect(init_action.action(), &QAction::triggered,
            this, &OSSIAApplicationPlugin::on_init);

    auto& ctrl = ctx.components.applicationPlugin<Scenario::ScenarioApplicationPlugin>();
    con(ctrl.execution(), &Scenario::ScenarioExecution::playAtDate,
        this, [=,act=play_action.action()] (const TimeValue& t)
    {
        on_play(true, t);
        act->trigger();
    });

    m_playActions.setupContextMenu(ctrl.layerContextMenuRegistrar());
}

OSSIAApplicationPlugin::~OSSIAApplicationPlugin()
{
    // The scenarios playing should already have been stopped by virtue of
    // aboutToClose.

    auto& children = m_localDevice->children();
    while(!children.empty())
        m_localDevice->erase(children.end() - 1);

    OSSIA::CleanupProtocols();
}

bool OSSIAApplicationPlugin::handleStartup()
{
    if(!context.documents.documents().empty())
    {
        if(context.applicationSettings.autoplay)
        {
            // TODO what happens if we load multiple documents ?
            on_play(true);
            return true;
        }
    }

    return false;
}

void OSSIAApplicationPlugin::on_newDocument(iscore::Document* doc)
{
    doc->model().addPluginModel(new Ossia::LocalTree::DocumentPlugin{m_localDevice,*doc, &doc->model()});
    doc->model().addPluginModel(new RecreateOnPlay::DocumentPlugin{*doc, &doc->model()});
}

void OSSIAApplicationPlugin::on_loadedDocument(iscore::Document *doc)
{
    on_newDocument(doc);
}

void OSSIAApplicationPlugin::on_documentChanged(
        iscore::Document* olddoc,
        iscore::Document* newdoc)
{
    if(olddoc)
    {
        // Disable the local tree for this document by removing
        // the node temporarily
        /*
        auto& doc_plugin = olddoc->context().plugin<DeviceDocumentPlugin>();
        doc_plugin.setConnection(false);
        */
    }

    if(newdoc)
    {
        // Enable the local tree for this document.

        /*
        auto& doc_plugin = newdoc->context().plugin<DeviceDocumentPlugin>();
        doc_plugin.setConnection(true);
        */
    }
}

void OSSIAApplicationPlugin::on_play(bool b, ::TimeValue t)
{
    // TODO have a on_exit handler to properly stop the scenario.
    if(auto doc = currentDocument())
    {
        auto scenar = dynamic_cast<Scenario::ScenarioDocumentModel*>(&doc->model().modelDelegate());
        if(!scenar)
            return;
        on_play(scenar->displayedElements.constraint(), b, t);
    }
}

void OSSIAApplicationPlugin::on_play(Scenario::ConstraintModel& cst, bool b, TimeValue t)
{
    auto doc = currentDocument();
    ISCORE_ASSERT(doc);

    auto plugmodel = doc->context().findPlugin<RecreateOnPlay::DocumentPlugin>();
    if(!plugmodel)
        return;

    if(b)
    {
        if(m_playing)
        {
            ISCORE_ASSERT(bool(m_clock));
            auto bs = plugmodel->baseScenario();
            auto& cstr = *bs->baseConstraint()->OSSIAConstraint();
            if(cstr.paused())
            {
                m_clock->resume();
            }
        }
        else
        {
            // Here we stop the listening when we start playing the scenario.
            // Get all the selected nodes
            auto explorer = Explorer::try_deviceExplorerFromObject(*doc);
            // Disable listening for everything
            if(explorer)
                explorer->deviceModel().listening().stop();

            plugmodel->reload(cst);

            m_clock = makeClock(plugmodel->context());

            connect(plugmodel->baseScenario(), &RecreateOnPlay::BaseScenarioElement::finished,
                    this, [=] () {
                // TODO change the action icon state
                on_stop();
            }, Qt::QueuedConnection);
            m_clock->play(t);
        }

        m_playing = true;
    }
    else
    {
        if(m_clock)
        {
            m_clock->pause();
        }
    }
}

void OSSIAApplicationPlugin::on_record(::TimeValue t)
{
    ISCORE_ASSERT(!m_playing);

    // TODO have a on_exit handler to properly stop the scenario.
    if(auto doc = currentDocument())
    {
        auto plugmodel = doc->context().findPlugin<RecreateOnPlay::DocumentPlugin>();
        if(!plugmodel)
            return;
        auto scenar = dynamic_cast<Scenario::ScenarioDocumentModel*>(&doc->model().modelDelegate());
        if(!scenar)
            return;

        // Listening isn't stopped here.
        plugmodel->reload(scenar->baseConstraint());
        m_clock = makeClock(plugmodel->context());
        m_clock->play(t);

        m_playing = true;
    }
}

void OSSIAApplicationPlugin::on_stop()
{
    if(auto doc = currentDocument())
    {
        auto plugmodel = doc->context().findPlugin<RecreateOnPlay::DocumentPlugin>();
        if(!plugmodel)
            return;

        if(plugmodel && plugmodel->baseScenario())
        {
            m_playing = false;

            m_clock->stop();
            m_clock.reset();
            plugmodel->clear();
        }

        // If we can we resume listening
        if(!context.documents.preparingNewDocument())
        {
            auto explorer = Explorer::try_deviceExplorerFromObject(*doc);
            if(explorer)
                explorer->deviceModel().listening().restore();
        }
    }
}

void OSSIAApplicationPlugin::on_init()
{
    if(auto doc = currentDocument())
    {
        auto plugmodel = doc->context().findPlugin<RecreateOnPlay::DocumentPlugin>();
        if(!plugmodel)
            return;

        auto scenar = dynamic_cast<Scenario::ScenarioDocumentModel*>(&doc->model().modelDelegate());
        if(!scenar)
            return;

        auto explorer = Explorer::try_deviceExplorerFromObject(*doc);
        // Disable listening for everything
        if(explorer)
            explorer->deviceModel().listening().stop();

        // FIXME this is terribly inefficient; we should just recreate the state...
        plugmodel->reload(scenar->baseConstraint());

        auto& st = *plugmodel->baseScenario()->startState();
        st.OSSIAState()->launch();

        plugmodel->clear();

        // If we can we resume listening
        if(!context.documents.preparingNewDocument())
        {
            auto explorer = Explorer::try_deviceExplorerFromObject(*doc);
            if(explorer)
                explorer->deviceModel().listening().restore();
        }
    }
}

void OSSIAApplicationPlugin::setupOSSIACallbacks()
{
    if(!m_localDevice)
        return;
    auto& dev = *m_localDevice;
    auto& children = dev.children();
    {
        auto end = children.cend();
        auto local_play_node = *(m_localDevice->emplace(end, "play"));
        auto local_play_address = local_play_node->createAddress(OSSIA::Value::Type::BOOL);
        local_play_address->setValue(new OSSIA::Bool{false});
        local_play_address->addCallback([&] (const OSSIA::Value* v) {
            if (v->getType() == OSSIA::Value::Type::BOOL)
            {
                on_play(static_cast<const OSSIA::Bool*>(v)->value);
            }
        });
    }
    {
        auto end = children.cend();
        auto local_stop_node = *(m_localDevice->emplace(end, "stop"));
        auto local_stop_address = local_stop_node->createAddress(OSSIA::Value::Type::IMPULSE);
        local_stop_address->setValue(new OSSIA::Impulse{});
        local_stop_address->addCallback([&] (const OSSIA::Value*) {
            on_stop();
        });

    }

    auto remote_protocol = OSSIA::Minuit::create("127.0.0.1", 9999, 6666);
    m_remoteDevice = OSSIA::Device::create(remote_protocol, "i-score-remote");
}


std::unique_ptr<RecreateOnPlay::ClockManager> OSSIAApplicationPlugin::makeClock(
        const RecreateOnPlay::Context& ctx)
{
    auto& s = context.settings<RecreateOnPlay::Settings::Model>();
    return s.makeClock(ctx);
}
