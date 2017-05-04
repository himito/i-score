#include <algorithm>

#include "ZoomPolicy.hpp"
#include <Process/ZoomHelper.hpp>

ZoomRatio ZoomPolicy::sliderPosToZoomRatio(
    double sliderPos, double cstrMaxTime, int cstrMaxWidth)
{
  // sliderPos is in [0;1] : 0 is zoom max, 1 zoom min.
  /*    auto zMax = std::max(
                          96.,
                          20 + cstrMaxTime/cstrMaxWidth
                          );
  */
  auto zMax = 1.1 * cstrMaxTime / cstrMaxWidth;

  auto mapZoom = [](double val, double min, double max) {
    return (max - min) * val + min;
  };

  auto zMin = zMax * 0.001;
  return mapZoom(1 - sliderPos, zMin, zMax);
}

double ZoomPolicy::zoomRatioToSliderPos(
    ZoomRatio& z, double cstrMaxTime, int cstrMaxWidth)
{
  ZoomRatio zMax = cstrMaxTime / cstrMaxWidth * 1.1;
  if (z == 0)
    z = zMax;

  auto zMin = zMax * 0.001;
  return ((zMax - z) / (zMax - zMin));
}
