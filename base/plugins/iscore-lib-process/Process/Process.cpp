#include <QObject>
#include <algorithm>
#include <stdexcept>

#include "Process.hpp"
#include <ossia/detail/algorithms.hpp>
#include <Process/ExpandMode.hpp>
#include <Process/TimeValue.hpp>
#include <iscore/model/ModelMetadata.hpp>
#include <iscore/model/IdentifiedObject.hpp>
#include <iscore/model/Identifier.hpp>

template class IdentifiedObject<Process::ProcessModel>;
template class iscore::SerializableInterface<Process::ProcessModelFactory>;
namespace Process
{
ProcessModel::ProcessModel(
    TimeVal duration,
    const Id<ProcessModel>& id,
    const QString& name,
    QObject* parent)
    : Entity{id, name, parent}
    , m_duration{std::move(duration)}
    , m_slotHeight{300}
{
}

ProcessModel::~ProcessModel()
{
  emit identified_object_destroying(this);
}

ProcessModel::ProcessModel(
    const ProcessModel& source,
    const Id<ProcessModel>& id,
    const QString& name,
    QObject* parent)
    : Entity{source, id, name, parent}
    , m_duration{source.duration()}
    , m_slotHeight{source.m_slotHeight}
{
}

void ProcessModel::setDurationAndScale(const TimeVal& newDuration)
{
  setDuration(newDuration);
}

void ProcessModel::setDurationAndGrow(const TimeVal& newDuration)
{
  setDuration(newDuration);
}

void ProcessModel::setDurationAndShrink(const TimeVal& newDuration)
{
  setDuration(newDuration);
}

ProcessModel::ProcessModel(DataStream::Deserializer& vis, QObject* parent)
  : Entity(vis, parent)
{
  vis.writeTo(*this);
}

ProcessModel::ProcessModel(JSONObject::Deserializer& vis, QObject* parent)
    : Entity(vis, parent)
{
  vis.writeTo(*this);
}

QString ProcessModel::prettyName() const
{
  return metadata().getName();
}

void ProcessModel::setParentDuration(ExpandMode mode, const TimeVal& t)
{
  switch (mode)
  {
    case ExpandMode::Scale:
      setDurationAndScale(t);
      break;
    case ExpandMode::GrowShrink:
    {
      if (duration() < t)
        setDurationAndGrow(t);
      else
        setDurationAndShrink(t);
      break;
    }
    case ExpandMode::ForceGrow:
    {
      if (duration() < t)
        setDurationAndGrow(t);
      break;
    }
    case ExpandMode::CannotExpand:
    default:
      break;
  }
}

void ProcessModel::setDuration(const TimeVal& other)
{
  m_duration = other;
  emit durationChanged(m_duration);
}

const TimeVal& ProcessModel::duration() const
{
  return m_duration;
}

void ProcessModel::startExecution()
{
}

void ProcessModel::stopExecution()
{
}

void ProcessModel::reset()
{
}

ProcessStateDataInterface*ProcessModel::startStateData() const
{
  return nullptr;
}

ProcessStateDataInterface*ProcessModel::endStateData() const
{
  return nullptr;
}

Selection ProcessModel::selectableChildren() const
{
  return {};
}

Selection ProcessModel::selectedChildren() const
{
  return {};
}

void ProcessModel::setSelection(const Selection& s) const
{
}

double ProcessModel::getSlotHeight() const
{
  return m_slotHeight;
}

void ProcessModel::setSlotHeight(double v)
{
  m_slotHeight = v;
  emit slotHeightChanged(v);
}

ProcessModel* parentProcess(QObject* obj)
{
  QString objName(obj ? obj->objectName() : "INVALID");
  while (obj && !dynamic_cast<ProcessModel*>(obj))
  {
    obj = obj->parent();
  }

  if (!obj)
    throw std::runtime_error(
        QString("Object (name: %1) is not child of a Process!")
            .arg(objName)
            .toStdString());

  return static_cast<ProcessModel*>(obj);
}

const ProcessModel* parentProcess(const QObject* obj)
{
  QString objName(obj ? obj->objectName() : "INVALID");
  while (obj && !dynamic_cast<const ProcessModel*>(obj))
  {
    obj = obj->parent();
  }

  if (!obj)
    throw std::runtime_error(
        QString("Object (name: %1) is not child of a Process!")
            .arg(objName)
            .toStdString());

  return static_cast<const ProcessModel*>(obj);
}
}
