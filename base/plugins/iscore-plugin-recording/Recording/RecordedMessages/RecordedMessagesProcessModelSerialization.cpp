#include <QJsonObject>
#include <QJsonValue>
#include <QString>

#include "RecordedMessagesProcessModel.hpp"
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONValueVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>

template <typename T>
class Reader;
template <typename T>
class Writer;


template <>
void DataStreamReader::read(
    const RecordedMessages::RecordedMessage& rm)
{
  m_stream << rm.percentage << rm.message;
}


template <>
void DataStreamWriter::write(
    RecordedMessages::RecordedMessage& rm)
{
  m_stream >> rm.percentage >> rm.message;
}


template <>
void JSONObjectReader::read(
    const RecordedMessages::RecordedMessage& rm)
{
  obj["Percentage"] = rm.percentage;
  obj[strings.Message] = toJsonObject(rm.message);
}


template <>
void JSONObjectWriter::write(
    RecordedMessages::RecordedMessage& rm)
{
  rm.percentage = obj["Percentage"].toDouble();
  rm.message = fromJsonObject<State::Message>(obj[strings.Message]);
}


template <>
void DataStreamReader::read(
    const RecordedMessages::ProcessModel& proc)
{
  m_stream << proc.m_messages;

  insertDelimiter();
}


template <>
void DataStreamWriter::write(RecordedMessages::ProcessModel& proc)
{
  m_stream >> proc.m_messages;

  checkDelimiter();
}


template <>
void JSONObjectReader::read(
    const RecordedMessages::ProcessModel& proc)
{
  obj["Messages"] = toJsonArray(proc.messages());
}


template <>
void JSONObjectWriter::write(RecordedMessages::ProcessModel& proc)
{
  fromJsonArray(obj["Messages"].toArray(), proc.m_messages);
}
