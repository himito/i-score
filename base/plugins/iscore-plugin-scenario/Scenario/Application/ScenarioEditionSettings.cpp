#include "ScenarioEditionSettings.hpp"
#include <Process/ExpandMode.hpp>
#include <Scenario/Palette/Tool.hpp>

ExpandMode Scenario::EditionSettings::expandMode() const
{
  return m_expandMode;
}

Scenario::Tool Scenario::EditionSettings::tool() const
{
  return m_tool;
}

void Scenario::EditionSettings::setExpandMode(ExpandMode expandMode)
{
  if (m_expandMode == expandMode)
    return;

  m_expandMode = expandMode;
  emit expandModeChanged(expandMode);
}

void Scenario::EditionSettings::setTool(Scenario::Tool tool)
{
  if (m_execution)
    return;

  if(tool != m_tool)
  {
    if (m_tool != Scenario::Tool::Playing)
      m_previousTool = m_tool;

    if (m_tool != Scenario::Tool::Create)
      setSequence(false);

    m_tool = tool;
    emit toolChanged(tool);
  }
}

bool Scenario::EditionSettings::sequence() const
{
  return m_sequence;
}

void Scenario::EditionSettings::setSequence(bool sequence)
{
  if (m_sequence == sequence)
    return;

  m_sequence = sequence;
  emit sequenceChanged(sequence);
}

void Scenario::EditionSettings::setExecution(bool ex)
{
  m_execution = ex;
}

void Scenario::EditionSettings::setDefault()
{
  setTool(m_previousTool);
  setSequence(false);
}

void Scenario::EditionSettings::restoreTool()
{
  setTool(Scenario::Tool{m_previousTool});
  if (m_tool != Scenario::Tool::Create)
    setSequence(false);
}
