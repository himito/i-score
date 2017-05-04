#pragma once
#include <iscore/plugins/customfactory/FactoryInterface.hpp>
#include <iscore/statemachine/GraphicsSceneToolPalette.hpp>
#include <memory>

namespace Scenario
{
class ConstraintModel;
class ScenarioDocumentPresenter;

class DisplayedElementsToolPaletteFactory
    : public iscore::Interface<DisplayedElementsToolPaletteFactory>
{
  ISCORE_INTERFACE("0884edb9-81e4-44ff-856f-fbc772f6d683")
public:
  virtual ~DisplayedElementsToolPaletteFactory();

  virtual bool matches(const ConstraintModel& constraint) const = 0;

  bool matches(
      ScenarioDocumentPresenter& pres, const ConstraintModel& constraint) const
  {
    return matches(constraint);
  }

  virtual std::unique_ptr<GraphicsSceneToolPalette>
  make(ScenarioDocumentPresenter& pres, const ConstraintModel& constraint) = 0;
};
}
