#include <Interpolation/InterpolationColors.hpp>
#include <iscore/model/Skin.hpp>

namespace Interpolation
{
Colors::Colors(const iscore::Skin& s)
    : m_style{s.Emphasis4,
              s.Smooth3,
              s.Tender3,
              s.Smooth3, s.Gray}
{
  m_style.init(s);
}
}
