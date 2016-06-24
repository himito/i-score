#include <Scenario/Process/Temporal/TemporalScenarioLayerModel.hpp>
#include <Scenario/Process/Temporal/TemporalScenarioPresenter.hpp>
#include <Scenario/Process/Temporal/TemporalScenarioView.hpp>

#include <iscore/tools/std/Optional.hpp>
#include <QApplication>
#include <QRect>
#include <algorithm>
#include <vector>

#include <Scenario/Application/ScenarioEditionSettings.hpp>
#include <Scenario/Palette/Tool.hpp>
#include <Scenario/Palette/Tools/CreationToolState.hpp>
#include <Scenario/Palette/Tools/MoveSlotToolState.hpp>
#include <Scenario/Palette/Tools/SmartTool.hpp>
#include <Scenario/Palette/Tools/States/ScenarioMoveStatesWrapper.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include "ScenarioPalette.hpp"
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/plugins/customfactory/StringFactoryKey.hpp>
#include <iscore/statemachine/GraphicsSceneToolPalette.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <Scenario/Document/State/ItemModel/MessageItemModel.hpp>

namespace Scenario
{
ToolPalette::ToolPalette(
        Process::LayerContext& lay,
        TemporalScenarioPresenter& presenter):
    GraphicsSceneToolPalette{*presenter.view().scene()},
    m_presenter{presenter},
    m_model{static_cast<const Scenario::ScenarioModel&>(m_presenter.m_layer.processModel())},
    m_context{lay},
    m_createTool{*this},
    m_selectTool{*this},
    m_playTool{*this},
    m_moveSlotTool{*this},
    m_inputDisp{presenter.view(), *this, lay}
{
}

Scenario::EditionSettings& ToolPalette::editionSettings() const
{
    return m_presenter.editionSettings();
}

void ToolPalette::on_pressed(QPointF point)
{
    scenePoint = point;
    auto scenarioPoint = ScenePointToScenarioPoint(m_presenter.m_view->mapFromScene(point));
    switch(editionSettings().tool())
    {
        case Scenario::Tool::Create:
            m_createTool.on_pressed(point, scenarioPoint);
            break;
        case Scenario::Tool::Playing:
        case Scenario::Tool::Select:
            m_selectTool.on_pressed(point, scenarioPoint);
            break;
        case Scenario::Tool::Play:
            m_playTool.on_pressed(point, scenarioPoint);
            break;
        case Scenario::Tool::MoveSlot:
            m_moveSlotTool.on_pressed(point);
            break;
        default:
            break;
    }

}

void ToolPalette::on_moved(QPointF point)
{
    scenePoint = point;
    auto scenarioPoint = ScenePointToScenarioPoint(m_presenter.m_view->mapFromScene(point));
    switch(editionSettings().tool())
    {
        case Scenario::Tool::Create:
            m_createTool.on_moved(point, scenarioPoint);
            break;
        case Scenario::Tool::Select:
            m_selectTool.on_moved(point, scenarioPoint);
            break;
        case Scenario::Tool::MoveSlot:
            m_moveSlotTool.on_moved();
            break;
        default:
            break;
    }

}

void ToolPalette::on_released(QPointF point)
{
    scenePoint = point;
    auto& es = m_presenter.editionSettings();
    auto scenarioPoint = ScenePointToScenarioPoint(m_presenter.m_view->mapFromScene(point));
    switch(es.tool())
    {
        case Scenario::Tool::Create:
            m_createTool.on_released(point, scenarioPoint);
            es.setTool(Scenario::Tool::Select);
            break;
        case Scenario::Tool::Playing:
            m_selectTool.on_released(point, scenarioPoint);
            es.setTool(Scenario::Tool::Select);
            break;
        case Scenario::Tool::Select:
            m_selectTool.on_released(point, scenarioPoint);
            break;
        case Scenario::Tool::MoveSlot:
            m_moveSlotTool.on_released();
            es.setTool(Scenario::Tool::Select);
            break;
        default:
            break;
    }

}

void ToolPalette::on_cancel()
{
    m_createTool.on_cancel();
    m_selectTool.on_cancel();
    m_moveSlotTool.on_cancel();
    m_presenter.editionSettings().setTool(Scenario::Tool::Select);
}

void ToolPalette::activate(Tool t)
{
    if(t == Scenario::Tool::MoveSlot)
        m_moveSlotTool.activate();
}

void ToolPalette::desactivate(Tool t)
{
    if(t == Scenario::Tool::MoveSlot)
        m_moveSlotTool.desactivate();
}

Scenario::Point ToolPalette::ScenePointToScenarioPoint(QPointF point)
{
    return ConvertToScenarioPoint(
                point,
                m_presenter.zoomRatio(),
                m_presenter.view().boundingRect().height());
}
}
