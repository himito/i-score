
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <algorithm>
#include <core/document/Document.hpp>
#include <vector>

#include <Recording/RecordedMessages/RecordedMessagesProcess.hpp>
#include <Recording/RecordedMessages/RecordedMessagesProcessMetadata.hpp>
#include <Recording/RecordedMessages/RecordedMessagesProcessModel.hpp>
#include <iscore/document/DocumentInterface.hpp>
#include <iscore/serialization/VisitorCommon.hpp>
#include <iscore/model/Identifier.hpp>

namespace RecordedMessages
{
ProcessModel::ProcessModel(
    const TimeVal& duration,
    const Id<Process::ProcessModel>& id,
    QObject* parent)
    : Process::ProcessModel{duration, id,
                            Metadata<ObjectKey_k, ProcessModel>::get(), parent}
{
  metadata().setInstanceName(*this);
}

ProcessModel::ProcessModel(
    const ProcessModel& source,
    const Id<Process::ProcessModel>& id,
    QObject* parent)
    : Process::ProcessModel{source, id,
                            Metadata<ObjectKey_k, ProcessModel>::get(), parent}
    , m_messages{source.m_messages}
{
  metadata().setInstanceName(*this);
}

ProcessModel::~ProcessModel()
{
}

void ProcessModel::setMessages(const QList<RecordedMessage>& script)
{
  m_messages = script;
  emit messagesChanged();
}

void ProcessModel::setDurationAndScale(const TimeVal& newDuration)
{
  setDuration(newDuration);
  emit messagesChanged();
}

void ProcessModel::setDurationAndGrow(const TimeVal& newDuration)
{
  int n = m_messages.size();
  auto ratio = duration() / newDuration;
  for (int i = 0; i < n; i++)
  {
    m_messages[i].percentage *= ratio;
  }
  setDuration(newDuration);
  emit messagesChanged();
}

void ProcessModel::setDurationAndShrink(const TimeVal& newDuration)
{
  auto ratio = duration() / newDuration;
  auto inv_ratio = newDuration / duration();

  QMutableListIterator<RecordedMessage> i(m_messages);
  while (i.hasNext())
  {
    auto& rm = i.next();
    if (rm.percentage >= inv_ratio)
      i.remove();
    else
      rm.percentage *= ratio;
  }
  setDuration(newDuration);
  emit messagesChanged();
}
}
