#include "AddTrigger.hpp"

#include "Document/TimeNode/TimeNodeModel.hpp"
#include "Document/TimeNode/Trigger/TriggerModel.hpp"

#include "Process/ScenarioModel.hpp"

using namespace Scenario::Command;

AddTrigger::AddTrigger(Path<TimeNodeModel>&& timeNodePath):
    iscore::SerializableCommand{
	factoryName(), commandName(), description()},
    m_path{std::move(timeNodePath)}
{

}

AddTrigger::~AddTrigger()
{
    qDeleteAll(m_cmds);
}

void AddTrigger::undo()
{
    auto& tn = m_path.find();
    tn.trigger()->setActive(false);

    for (auto cmd : m_cmds)
    {
        cmd->undo();
        delete cmd;
        m_cmds.removeAll(cmd);
    }
}

void AddTrigger::redo()
{
    auto& tn = m_path.find();
    tn.trigger()->setActive(true);

    ScenarioModel* scenar = dynamic_cast<ScenarioModel*>(tn.parentScenario());

    for (auto& cstrId : scenar->constraintsBeforeTimeNode(tn.id()))
    {
        auto cmd = new SetRigidity(iscore::IDocument::path(scenar->constraint(cstrId)), false);
        cmd->redo();
        m_cmds.push_back(cmd);
    }
}

void AddTrigger::serializeImpl(QDataStream& s) const
{
    s << m_path;
    s << m_cmds.count();

    for(const auto& cmd : m_cmds)
    {
        s << cmd->serialize();
    }
}

void AddTrigger::deserializeImpl(QDataStream& s)
{
    int n;
    s >> m_path;
    s >> n;
        for(;n-->0;)
        {
            QByteArray a;
            s >> a;
            auto cmd = new SetRigidity;
            cmd->deserialize(a);
            m_cmds.append(cmd);
        }
}
