#pragma once
#include <Process/StateProcessFactory.hpp>

#include <iscore/plugins/customfactory/FactoryFamily.hpp>

namespace Process
{
class ISCORE_LIB_PROCESS_EXPORT StateProcessList final
    : public iscore::InterfaceList<StateProcessFactory>
{
public:
  using object_type = Process::StateProcess;
  ~StateProcessList();
  object_type* loadMissing(const VisitorVariant& vis, QObject* parent) const;
};
}
