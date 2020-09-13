#pragma once

#include <stdexcept>
#include "elevation_reader.h"
#include "height_field.h"

namespace terrain
{
geo_reference::geo_reference(double* geo, double* inv, const int w, const int h)
  : mGeoToImageTransform(1.0)
  , mImageToGeoTransform(1.0)
  , mImageSize(w, h)
{
  if (!geo || !inv || w < 0 || h < 0)
  {
    throw std::runtime_error(std::string("invalid geo_reference creation parameter(s)."));
  }

  mImageToGeoTransform = glm::dmat3x2(geo[1], geo[2], geo[4],
    geo[5], geo[0], geo[3]);
  mGeoToImageTransform = glm::dmat3x2(inv[1], inv[2], inv[4],
    inv[5], inv[0], inv[3]);
}

uvec2 geo_reference::geoToImg(const geo& geo) const
{
  return glm::ivec2(mGeoToImageTransform * vec3(geo, 1.0));
}
geo geo_reference::imgToGeo(const uvec2& img) const
{
  return geo(mImageToGeoTransform * vec3(img, 1.0));
}
const uvec2& geo_reference::getPixelCount() const
{
  return mImageSize;
}
const geo geo_reference::getOrigin() const
{
  return geo(mImageToGeoTransform[2]);
}
const vec2 geo_reference::getResolution() const
{
  return vec2(mImageToGeoTransform[1]);
}
void geo_reference::clear()
{
  mGeoToImageTransform = mat32(1.0);
  mImageToGeoTransform = mat32(1.0);
  mImageSize = glm::ivec2(0, 0);
}
//geo_reference geo_reference::read(const std::string& /*tiffPath*/)
//{
//  return geo_reference();
//}
}
