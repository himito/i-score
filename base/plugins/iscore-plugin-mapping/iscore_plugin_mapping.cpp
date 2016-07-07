#include <Mapping/MappingColors.hpp>
#include <Mapping/MappingLayerModel.hpp>
#include <Mapping/MappingModel.hpp>
#include <Mapping/MappingPresenter.hpp>
#include <Mapping/MappingView.hpp>

#include <unordered_map>

#include <Inspector/InspectorWidgetFactoryInterface.hpp>
#include <Mapping/Commands/MappingCommandFactory.hpp>
#include <Mapping/MappingProcessMetadata.hpp>
#include <Process/ProcessFactory.hpp>
#include <iscore/plugins/customfactory/StringFactoryKey.hpp>
#include "iscore_plugin_mapping.hpp"

#include <iscore/plugins/customfactory/FactorySetup.hpp>
#include <Curve/Process/CurveProcessFactory.hpp>

namespace Mapping {
using MappingFactory =
    Curve::CurveProcessFactory_T<
Mapping::ProcessModel,
Mapping::LayerModel,
Mapping::MappingPresenter,
Mapping::MappingView,
Mapping::MappingColors>;
}

#if defined(ISCORE_LIB_INSPECTOR)
#include <Mapping/Inspector/MappingInspectorFactory.hpp>
#endif

#include <iscore_plugin_mapping_commands_files.hpp>

iscore_plugin_mapping::iscore_plugin_mapping() :
    QObject {}
{
}

iscore_plugin_mapping::~iscore_plugin_mapping()
{

}

std::vector<std::unique_ptr<iscore::FactoryInterfaceBase>> iscore_plugin_mapping::factories(
        const iscore::ApplicationContext& ctx,
        const iscore::AbstractFactoryKey& key) const
{
    using namespace Mapping;
    return instantiate_factories<
            iscore::ApplicationContext,
            TL<
            FW<Process::ProcessFactory,
                Mapping::MappingFactory>,
            FW<Process::InspectorWidgetDelegateFactory,
                MappingInspectorFactory>
            >>(ctx, key);
}

std::pair<const CommandParentFactoryKey, CommandGeneratorMap> iscore_plugin_mapping::make_commands()
{
    using namespace Mapping;
    std::pair<const CommandParentFactoryKey, CommandGeneratorMap> cmds{MappingCommandFactoryName(), CommandGeneratorMap{}};

    using Types = TypeList<
#include <iscore_plugin_mapping_commands.hpp>
      >;
    for_each_type<Types>(iscore::commands::FactoryInserter{cmds.second});

    return cmds;
}

iscore::Version iscore_plugin_mapping::version() const
{
    return iscore::Version{1};
}

UuidKey<iscore::Plugin> iscore_plugin_mapping::key() const
{
    return "e097f02d-4676-492e-98b0-764963e1f792";
}
