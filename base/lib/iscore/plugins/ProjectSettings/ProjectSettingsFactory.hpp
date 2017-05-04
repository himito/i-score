#pragma once
#include <iscore/plugins/customfactory/FactoryFamily.hpp>
#include <iscore_lib_base_export.h>
namespace iscore
{
class ProjectSettingsPresenter;
class ProjectSettingsModel;
class ProjectSettingsView;

/**
 * @brief The ProjectSettingsFactory class
 *
 * Reimplement in order to provide custom settings for the plug-in.
 */
class ISCORE_LIB_BASE_EXPORT ProjectSettingsFactory
    : public iscore::Interface<ProjectSettingsFactory>
{
  ISCORE_INTERFACE("18658b23-d20e-4a54-b16d-8f7072de9e9f")

public:
  virtual ~ProjectSettingsFactory();
  ProjectSettingsPresenter* makePresenter(
      iscore::ProjectSettingsModel& m,
      iscore::ProjectSettingsView& v,
      QObject* parent);
  virtual ProjectSettingsView* makeView() = 0;
  virtual ProjectSettingsModel* makeModel() = 0;

protected:
  virtual ProjectSettingsPresenter* makePresenter_impl(
      iscore::ProjectSettingsModel& m,
      iscore::ProjectSettingsView& v,
      QObject* parent)
      = 0;
};

class ISCORE_LIB_BASE_EXPORT ProjectSettingsFactoryList final
    : public InterfaceList<iscore::ProjectSettingsFactory>
{
public:
  using object_type = ProjectSettingsFactory;
};
}
