#include "SetProcessPosition.hpp"
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <iscore/model/path/PathSerialization.hpp>

namespace Scenario
{
namespace Command
{
PutProcessBefore::PutProcessBefore(
    Path<Scenario::ConstraintModel>&& cst,
    Id<Process::ProcessModel>
        proc,
    Id<Process::ProcessModel>
        proc2)
    : m_path{std::move(cst)}
    , m_proc{std::move(proc)}
    , m_proc2{std::move(proc2)}
{
}

void PutProcessBefore::undo() const
{
  redo();
}

void PutProcessBefore::redo() const
{
  auto& cst = m_path.find();
  cst.processes.relocate(m_proc, m_proc2);
}

void PutProcessBefore::serializeImpl(DataStreamInput& s) const
{
  s << m_path << m_proc << m_proc2;
}

void PutProcessBefore::deserializeImpl(DataStreamOutput& s)
{
  s >> m_path >> m_proc >> m_proc2;
}

PutProcessToEnd::PutProcessToEnd(
    Path<Scenario::ConstraintModel>&& cst, Id<Process::ProcessModel> proc)
    : m_path{std::move(cst)}, m_proc{std::move(proc)}
{
  auto& c = m_path.find();
  auto it = c.processes.find(m_proc);
  ISCORE_ASSERT(it != c.processes.end());
  std::advance(it, 1);
  ISCORE_ASSERT(it != c.processes.end());
  m_proc_after = it->id();
}

void PutProcessToEnd::undo() const
{
  auto& cst = m_path.find();
  cst.processes.relocate(m_proc, m_proc_after);
}

void PutProcessToEnd::redo() const
{
  auto& cst = m_path.find();
  cst.processes.putToEnd(m_proc);
}

void PutProcessToEnd::serializeImpl(DataStreamInput& s) const
{
  s << m_path << m_proc << m_proc_after;
}

void PutProcessToEnd::deserializeImpl(DataStreamOutput& s)
{
  s >> m_path >> m_proc >> m_proc_after;
}

SwapProcessPosition::SwapProcessPosition(
    Path<Scenario::ConstraintModel>&& cst,
    Id<Process::ProcessModel>
        proc,
    Id<Process::ProcessModel>
        proc2)
    : m_path{std::move(cst)}
    , m_proc{std::move(proc)}
    , m_proc2{std::move(proc2)}
{
}

void SwapProcessPosition::undo() const
{
  redo();
}

void SwapProcessPosition::redo() const
{
  auto& cst = m_path.find();
  cst.processes.swap(m_proc, m_proc2);
}

void SwapProcessPosition::serializeImpl(DataStreamInput& s) const
{
  s << m_path << m_proc << m_proc2;
}

void SwapProcessPosition::deserializeImpl(DataStreamOutput& s)
{
  s >> m_path >> m_proc >> m_proc2;
}
}
}
