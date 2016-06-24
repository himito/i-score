#include "LayerModel.hpp"
#include <iscore/tools/IdentifiedObject.hpp>

class QObject;
#include <iscore/tools/SettableIdentifier.hpp>
namespace Process
{
LayerModel::~LayerModel() = default;

ProcessModel& LayerModel::processModel() const
{ return m_sharedProcessModel; }


LayerModel::LayerModel(
        const Id<LayerModel>& viewModelId,
        const QString& name,
        ProcessModel& sharedProcess,
        QObject* parent) :
    IdentifiedObject<LayerModel> {viewModelId, name, parent},
    m_sharedProcessModel {sharedProcess}
{

}
}
