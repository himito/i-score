#include "MoveNotes.hpp"
#include <Midi/MidiProcess.hpp>
#include <iscore/model/path/PathSerialization.hpp>

namespace Midi
{

MoveNotes::MoveNotes(
    const ProcessModel& model,
    const std::vector<Id<Note>>& to_move,
    int note_delta,
    double t_delta)
    : m_model{model}
{
  m_before.reserve(to_move.size());
  m_after.reserve(to_move.size());
  for (auto& note_id : to_move)
  {
    auto& note = model.notes.at(note_id);
    NoteData data = note.noteData();
    m_before.push_back(qMakePair(note.id(), data));
    data.m_pitch = qBound(0, data.m_pitch + note_delta, 127);
    data.m_start = std::max(data.m_start + t_delta, 0.);
    m_after.push_back(qMakePair(note.id(), data));
  }
}

void MoveNotes::undo() const
{
  auto& model = m_model.find();
  for (const auto& note : m_before)
  {
    auto& n = model.notes.at(note.first);
    n.setStart(note.second.start());
    n.setPitch(note.second.pitch());
  }
}

void MoveNotes::redo() const
{
  auto& model = m_model.find();
  for (const auto& note : m_after)
  {
    auto& n = model.notes.at(note.first);
    n.setStart(note.second.start());
    n.setPitch(note.second.pitch());
  }
}

void MoveNotes::update(unused_t, unused_t, int note_delta, double t_delta)
{
  m_after.clear();
  m_after.reserve(m_before.size());
  for (auto& elt : m_before)
  {
    NoteData data = elt.second;
    data.setPitch(qBound(0, data.pitch() + note_delta, 127));
    data.setStart(std::max(data.start() + t_delta, 0.));
    m_after.push_back({elt.first, std::move(data)});
  }
}

void MoveNotes::serializeImpl(DataStreamInput& s) const
{
  s << m_model << m_before << m_after;
}

void MoveNotes::deserializeImpl(DataStreamOutput& s)
{
  s >> m_model >> m_before >> m_after;
}
}
