#pragma once

#include <QByteArray>
#include <QString>
#include <iscore/plugins/customfactory/FactoryInterface.hpp>
#include <iscore/serialization/VisitorCommon.hpp>
#include <iscore/model/Identifier.hpp>
#include <iscore_lib_process_export.h>

namespace Process
{
class StateProcess;

class ISCORE_LIB_PROCESS_EXPORT StateProcessFactory
    : public iscore::Interface<StateProcessFactory>
{
  ISCORE_INTERFACE("262852da-8662-482e-a7be-ee7896d13866")

public:
  virtual ~StateProcessFactory();
  virtual QString prettyName() const = 0;

  virtual StateProcess* make(const Id<StateProcess>& id, QObject* parent) = 0;

  // throws if the serialization method is not implemented by the subclass
  virtual StateProcess* load(const VisitorVariant&, QObject* parent) = 0;
};

template <typename Model_T>
class StateProcessFactory_T : public StateProcessFactory
{
public:
  QString prettyName() const override
  {
    return Metadata<PrettyName_k, Model_T>::get();
  }

  UuidKey<Process::StateProcessFactory> concreteKey() const noexcept override
  {
    return Metadata<ConcreteKey_k, Model_T>::get();
  }

  Model_T* make(const Id<StateProcess>& id, QObject* parent) override
  {
    return new Model_T{id, parent};
  }

  Model_T* load(const VisitorVariant& vis, QObject* parent) override
  {
    return deserialize_dyn(vis, [&](auto&& deserializer) {
      return new Model_T{deserializer, parent};
    });
  }
};
}
