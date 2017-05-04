#include "DataStreamVisitor.hpp"
#include <QIODevice>
#include <iscore/application/ApplicationContext.hpp>

template <typename T>
class Reader;
template <typename T>
class Writer;

DataStreamReader::DataStreamReader() : components{iscore::AppComponents()}
{
}

DataStreamReader::DataStreamReader(QByteArray* array)
    : m_stream_impl{array, QIODevice::WriteOnly}
    , components{iscore::AppComponents()}
{
  m_stream_impl.setVersion(QDataStream::Qt_5_3);
}

DataStreamReader::DataStreamReader(QIODevice* dev)
    : m_stream_impl{dev}, components{iscore::AppComponents()}
{
}

DataStreamWriter::DataStreamWriter() : components{iscore::AppComponents()}
{
}

DataStreamWriter::DataStreamWriter(const QByteArray& array)
    : m_stream_impl{array}, components{iscore::AppComponents()}
{
  m_stream_impl.setVersion(QDataStream::Qt_5_3);
}

DataStreamWriter::DataStreamWriter(QIODevice* dev)
    : m_stream_impl{dev}, components{iscore::AppComponents()}
{
}

QDataStream& operator<<(QDataStream& s, char c)
{
  return s << QChar(c);
}

QDataStream& operator>>(QDataStream& s, char& c)
{
  QChar r;
  s >> r;
  c = r.toLatin1();
  return s;
}

QDataStream& operator<<(QDataStream& stream, const std::string& obj)
{
  uint32_t size = obj.size();
  stream << size;

  stream.writeRawData(obj.data(), size);
  return stream;
}

QDataStream& operator>>(QDataStream& stream, std::string& obj)
{
  uint32_t n = 0;
  stream >> n;
  obj.resize(n);

  char* addr = n > 0 ? &obj[0] : nullptr;
  stream.readRawData(addr, n);

  return stream;
}
