#pragma once
#include <Process/ProcessFactory.hpp>

#include <iscore/plugins/customfactory/FactoryFamily.hpp>

namespace Process
{
class ISCORE_LIB_PROCESS_EXPORT ProcessFactoryList final
    : public iscore::InterfaceList<ProcessModelFactory>
{
public:
  using object_type = Process::ProcessModel;
  ~ProcessFactoryList();

  object_type* loadMissing(const VisitorVariant& vis, QObject* parent) const;
};

class ISCORE_LIB_PROCESS_EXPORT LayerFactoryList final
    : public iscore::InterfaceList<LayerFactory>
{
public:
  ~LayerFactoryList();

  LayerFactory* findDefaultFactory(const Process::ProcessModel& proc) const;
  LayerFactory* findDefaultFactory(
      const UuidKey<Process::ProcessModel>& proc) const;
  LayerFactory* get(
      const UuidKey<Process::ProcessModel>& proc) const { return findDefaultFactory(proc); }
};
}
