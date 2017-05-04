#pragma once
#include <QJsonDocument>
#include <State/Message.hpp>
#include <iscore/serialization/JSONVisitor.hpp>
#include <iscore/serialization/MimeVisitor.hpp>

namespace iscore
{
namespace mime
{
inline constexpr const char* messagelist()
{
  return "application/x-iscore-messagelist";
}
}
}

template <>
struct Visitor<Reader<Mime<State::MessageList>>> : public MimeDataReader
{
  using MimeDataReader::MimeDataReader;
  void serialize(const State::MessageList& lst) const
  {
    m_mime.setData(
        iscore::mime::messagelist(),
        QJsonDocument(toJsonArray(lst)).toJson(QJsonDocument::Indented));
  }
};

template <>
struct Visitor<Writer<Mime<State::MessageList>>> : public MimeDataWriter
{
  using MimeDataWriter::MimeDataWriter;
  auto deserialize()
  {
    State::MessageList ml;
    fromJsonArray(
        QJsonDocument::fromJson(m_mime.data(iscore::mime::messagelist()))
            .array(),
        ml);
    return ml;
  }
};
