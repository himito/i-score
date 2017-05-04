#include <QJsonObject>
#include <QJsonValue>
#include <iscore/serialization/DataStreamVisitor.hpp>
#include <iscore/serialization/JSONVisitor.hpp>

#include "SinSegment.hpp"


template <>
void DataStreamReader::read(const Curve::SinSegment& segmt)
{
  m_stream << segmt.freq << segmt.ampl;
}


template <>
void DataStreamWriter::write(Curve::SinSegment& segmt)
{
  m_stream >> segmt.freq >> segmt.ampl;
}


template <>
void JSONObjectReader::read(const Curve::SinSegment& segmt)
{
  obj["Freq"] = segmt.freq;
  obj["Ampl"] = segmt.ampl;
}


template <>
void JSONObjectWriter::write(Curve::SinSegment& segmt)
{
  segmt.freq = obj["Freq"].toDouble();
  segmt.ampl = obj["Ampl"].toDouble();
}


template <>
void DataStreamReader::read(const Curve::SinSegmentData& segmt)
{
  m_stream << segmt.freq << segmt.ampl;
}


template <>
void DataStreamWriter::write(Curve::SinSegmentData& segmt)
{
  m_stream >> segmt.freq >> segmt.ampl;
}


template <>
void JSONObjectReader::read(const Curve::SinSegmentData& segmt)
{
  obj["Freq"] = segmt.freq;
  obj["Ampl"] = segmt.ampl;
}


template <>
void JSONObjectWriter::write(Curve::SinSegmentData& segmt)
{
  segmt.freq = obj["Freq"].toDouble();
  segmt.ampl = obj["Ampl"].toDouble();
}
