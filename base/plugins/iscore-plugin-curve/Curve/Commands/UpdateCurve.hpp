#pragma once
#include <Curve/Commands/CurveCommandFactory.hpp>
#include <Curve/Segment/CurveSegmentData.hpp>
#include <iscore/command/Command.hpp>
#include <iscore/model/path/Path.hpp>
#include <vector>

struct DataStreamInput;
struct DataStreamOutput;

namespace Curve
{
class Model;
class UpdateCurve final : public iscore::Command
{
  ISCORE_COMMAND_DECL(CommandFactoryName(), UpdateCurve, "Update Curve")
public:
  UpdateCurve(Path<Model>&& model, std::vector<SegmentData>&& segments);

  void undo() const override;
  void redo() const override;

  void update(unused_t, std::vector<SegmentData>&& segments)
  {
    m_newCurveData = std::move(segments);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override;
  void deserializeImpl(DataStreamOutput& s) override;

private:
  Path<Model> m_model;
  std::vector<SegmentData> m_oldCurveData;
  std::vector<SegmentData> m_newCurveData;
};
}
