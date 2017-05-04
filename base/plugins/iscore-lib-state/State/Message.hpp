#pragma once
#include <QObject>
#include <QStringList>
#include <State/Address.hpp>
#include <State/Value.hpp>
#include <State/ValueConversion.hpp>
namespace State
{
/**
 * @brief The Message struct
 *
 * A message is an Address associated with a value :
 *
 *  aDevice:/aNode/anotherNode 2345
 *
 */
struct Message
{
  Message() = default;
  Message(State::AddressAccessor addr, State::Value val)
      : address(std::move(addr)), value(std::move(val))
  {
  }

  Message(const Message&) = default;
  Message(Message&&) = default;
  Message& operator=(const Message&) = default;
  Message& operator=(Message&&) = default;

  bool operator==(const Message& m) const
  {
    return address == m.address && value == m.value;
  }

  bool operator!=(const Message& m) const
  {
    return address != m.address && value != m.value;
  }

  bool operator<(const Message& m) const
  {
    return false;
  }

  QString toString() const
  {
    return address.toString() + " " + State::convert::toPrettyString(value);
  }

  friend QDebug operator<<(QDebug s, const State::Message& mess)
  {
    s << mess.toString();
    return s;
  }

  AddressAccessor address;
  Value value;
};

using MessageList = QList<Message>;
inline bool operator<(const State::MessageList&, const State::MessageList&)
{
  return false;
}
}

#include <iscore/serialization/JSONVisitor.hpp>
template <>
struct TSerializer<JSONObject, State::MessageList>
{
  static void
  readFrom(JSONObject::Serializer& s, const State::MessageList& obj)
  {
    s.obj[s.strings.Data] = toJsonArray(obj);
  }

  static void writeTo(JSONObject::Deserializer& s, State::MessageList& obj)
  {
    State::MessageList t;
    fromJsonArray(s.obj[s.strings.Data].toArray(), t);
    obj = t;
  }
};

Q_DECLARE_METATYPE(State::Message)
Q_DECLARE_METATYPE(State::MessageList)
