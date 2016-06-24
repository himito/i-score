#include <Process/Process.hpp>
#include <algorithm>

#include <Process/TimeValue.hpp>
#include "SetProcessDuration.hpp"
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/tools/ModelPathSerialization.hpp>

namespace Scenario
{
namespace Command
{

SetProcessDuration::SetProcessDuration(
        Path<Process::ProcessModel>&& path,
        TimeValue newVal) :
    m_path {std::move(path)},
    m_new {std::move(newVal)}
{
    m_old = m_path.find().duration();
}

void SetProcessDuration::undo() const
{
    m_path.find().setDuration(m_old);
}

void SetProcessDuration::redo() const
{
    m_path.find().setDuration(m_new);
}

void SetProcessDuration::serializeImpl(DataStreamInput& s) const
{
    s << m_path << m_old << m_new;
}

void SetProcessDuration::deserializeImpl(DataStreamOutput& s)
{
    s >> m_path >> m_old >> m_new;
}

}
}
