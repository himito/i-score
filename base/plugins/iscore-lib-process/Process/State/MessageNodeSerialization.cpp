
#include <QDataStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QVector>
#include <QtGlobal>
#include <algorithm>
#include <array>
#include <cstddef>
#include <iscore/serialization/VisitorCommon.hpp>

#include "MessageNode.hpp"
#include <State/Value.hpp>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONValueVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/model/Identifier.hpp>

namespace Process
{
class ProcessModel;
}
namespace boost
{
template <class T>
class optional;
} // namespace boost

template <typename T>
void toJsonValue(
    QJsonObject& object, const QString& name, const optional<T>& value)
{
  if (value)
  {
    object[name] = marshall<JSONObject>(*value);
  }
}

template <typename T, std::size_t N>
QJsonArray toJsonArray(const std::array<T, N>& array)
{
  QJsonArray arr;
  for (std::size_t i = 0; i < N; i++)
  {
    arr.append(toJsonValue(array.at(i)));
  }

  return arr;
}

template <typename T, std::size_t N>
void fromJsonArray(const QJsonArray& array, std::array<T, N>& res)
{
  for (std::size_t i = 0; i < N; i++)
  {
    res.at(i) = fromJsonValue<T>(array.at(i));
  }
}

template <typename T>
void fromJsonValue(
    const QJsonObject& object, const QString& name, optional<T>& value)
{
  auto it = object.find(name);
  if (it != object.end())
  {
    value = unmarshall<State::Value>((*it).toObject());
  }
  else
  {
    value = ossia::none;
  }
}

template <>
void DataStreamReader::read(
    const std::array<Process::PriorityPolicy, 3>& val)
{
  for (int i = 0; i < 3; i++)
    m_stream << val.at(i);
}

template <>
void DataStreamWriter::write(
    std::array<Process::PriorityPolicy, 3>& val)
{
  for (int i = 0; i < 3; i++)
    m_stream >> val.at(i);
}

template <>
void DataStreamReader::read(
    const Process::ProcessStateData& val)
{
  m_stream << val.process << val.value;
}

template <>
void DataStreamWriter::write(Process::ProcessStateData& val)
{
  m_stream >> val.process >> val.value;
}

template <>
void JSONObjectReader::read(
    const Process::ProcessStateData& val)
{
  obj[strings.Process] = toJsonValue(val.process);
  toJsonValue(obj, strings.Value, val.value);
}

template <>
void JSONObjectWriter::write(Process::ProcessStateData& val)
{
  val.process
      = fromJsonValue<Id<Process::ProcessModel>>(obj[strings.Process]);
  fromJsonValue(obj, strings.Value, val.value);
}

template <>
void DataStreamReader::read(const Process::StateNodeValues& val)
{
  m_stream << val.previousProcessValues << val.followingProcessValues
           << val.userValue << val.priorities;
}

template <>
void DataStreamWriter::write(Process::StateNodeValues& val)
{
  m_stream >> val.previousProcessValues >> val.followingProcessValues
      >> val.userValue >> val.priorities;
}

template <>
void JSONObjectReader::read(const Process::StateNodeValues& val)
{
  obj[strings.Previous] = toJsonArray(val.previousProcessValues);
  obj[strings.Following] = toJsonArray(val.followingProcessValues);
  toJsonValue(obj, strings.User, val.userValue);
  obj[strings.Priorities] = toJsonArray(val.priorities);
}

template <>
void JSONObjectWriter::write(Process::StateNodeValues& val)
{
  fromJsonArray(obj[strings.Previous].toArray(), val.previousProcessValues);
  fromJsonArray(
      obj[strings.Following].toArray(), val.followingProcessValues);
  fromJsonValue(obj, strings.User, val.userValue);
  fromJsonArray(obj[strings.Priorities].toArray(), val.priorities);
}

template <>
ISCORE_LIB_PROCESS_EXPORT void
DataStreamReader::read(const Process::StateNodeData& node)
{
  m_stream << node.name << node.values;
  insertDelimiter();
}

template <>
ISCORE_LIB_PROCESS_EXPORT void
DataStreamWriter::write(Process::StateNodeData& node)
{
  m_stream >> node.name >> node.values;
  checkDelimiter();
}

template <>
ISCORE_LIB_PROCESS_EXPORT void
JSONObjectReader::read(const Process::StateNodeData& node)
{
  readFrom(node.name);
  readFrom(node.values);
}

template <>
ISCORE_LIB_PROCESS_EXPORT void
JSONObjectWriter::write(Process::StateNodeData& node)
{
  writeTo(node.name);
  writeTo(node.values);
}
