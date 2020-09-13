#pragma once

#include <stdexcept>
#include "height_field.h"

namespace terrain
{
  using geo = vec2; // x = lon, y = lat
  
  class geo_reference
  {
  public:
    geo_reference(double* geo, double* inv, const int w, const int h);

  public:
    uvec2 geoToImg(const geo& geo) const;
    geo imgToGeo(const uvec2& img) const;

    const uvec2& getPixelCount() const;
    const geo getOrigin() const;
    const vec2 getResolution() const;
    void clear();

  public:
    static geo_reference read(const std::string& tiffPath);

  private:
    mat32 mGeoToImageTransform;
    mat32 mImageToGeoTransform;
    uvec2 mImageSize;
  };
}
