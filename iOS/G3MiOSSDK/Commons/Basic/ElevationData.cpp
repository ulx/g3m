//
//  ElevationData.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 2/17/13.
//
//

#include "ElevationData.hpp"

#include "Vector2I.hpp"

#include "FloatBufferBuilderFromGeodetic.hpp"
#include "FloatBufferBuilderFromColor.hpp"
#include "DirectMesh.hpp"
#include "GLConstants.hpp"

#include "BilinearInterpolator.hpp"

ElevationData::ElevationData(const Sector& sector,
                             const Vector2I& extent) :
_sector(sector),
_width(extent._x),
_height(extent._y),
_resolution(sector._deltaLatitude.div(extent._y),
            sector._deltaLongitude.div(extent._x)),
_interpolator(NULL)
{
}

ElevationData::~ElevationData() {
  if (_interpolator != NULL) {
    delete _interpolator;
  }
}

double ElevationData::getElevationAt(const Vector2I& position) const {
  return getElevationAt(position._x, position._y);
}

const Vector2I ElevationData::getExtent() const {
  return Vector2I(_width, _height);
}

Mesh* ElevationData::createMesh(const Planet* planet,
                                float verticalExaggeration,
                                const Geodetic3D& positionOffset,
                                float pointSize,
                                const Sector& sector,
                                const Vector2I& resolution) const {
  const Vector3D minMaxAverageElevations = getMinMaxAverageElevations();
  const double minElevation     = minMaxAverageElevations._x;
  const double maxElevation     = minMaxAverageElevations._y;
  const double deltaElevation   = maxElevation - minElevation;
  const double averageElevation = minMaxAverageElevations._z;

  ILogger::instance()->logInfo("Elevations: average=%f, min=%f max=%f delta=%f",
                               averageElevation, minElevation, maxElevation, deltaElevation);

  
//  FloatBufferBuilderFromGeodetic vertices(CenterStrategy::firstVertex(),
//                                          ellipsoid,
//                                          Vector3D::zero);
//  FloatBufferBuilderFromGeodetic vertices(CenterStrategy::givenCenter(),
//                                          planet,
//                                          sector._center);
  FloatBufferBuilderFromGeodetic vertices = FloatBufferBuilderFromGeodetic::builderWithGivenCenter(planet, sector._center);


  FloatBufferBuilderFromColor colors;

  const Geodetic2D positionOffset2D = positionOffset.asGeodetic2D();

  const int width  = resolution._x;
  const int height = resolution._y;
  for (int x = 0; x < width; x++) {
    const double u = (double) x / (width  - 1);

    for (int y = 0; y < height; y++) {
      const double v = 1.0 - ( (double) y / (height - 1) );

      const Geodetic2D position = sector.getInnerPoint(u, v);

      const double elevation = getElevationAt(position);
      if ( ISNAN(elevation) ) {
        continue;
      }

      const float alpha = (float) ((elevation - minElevation) / deltaElevation);
      const float r = alpha;
      const float g = alpha;
      const float b = alpha;
      colors.add(r, g, b, 1);

      vertices.add(position.add(positionOffset2D),
                   positionOffset._height + (elevation * verticalExaggeration));
    }
  }

//  for (int x = 0; x < _width; x++) {
//    const double u = (double) x / (_width  - 1);
//
//    for (int y = 0; y < _height; y++) {
//      const double v = 1.0 - ( (double) y / (_height - 1) );
//      const Geodetic2D position = _sector.getInnerPoint(u, v);
//      if (!sector.contains(position)) {
//        continue;
//      }
//
//      const double elevation = getElevationAt(x, y);
//      if (mu->isNan(elevation)) {
//        continue;
//      }
//
//      vertices.add(position.add(positionOffset2D),
//                   positionOffset._height + (elevation * verticalExaggeration));
//
//
//      const float alpha = (float) ((elevation - minElevation) / deltaElevation);
//      const float r = alpha;
//      const float g = alpha;
//      const float b = alpha;
//      colors.add(r, g, b, 1);
//    }
//  }


  const float lineWidth = 1;
  Color* flatColor = NULL;

  return new DirectMesh(GLPrimitive::points(),
                        //GLPrimitive::lineStrip(),
                        true,
                        vertices.getCenter(),
                        vertices.create(),
                        lineWidth,
                        pointSize,
                        flatColor,
                        colors.create(),
                        0,
                        false);
}

Mesh* ElevationData::createMesh(const Planet* planet,
                                float verticalExaggeration,
                                const Geodetic3D& positionOffset,
                                float pointSize) const {

  const Vector3D minMaxAverageElevations = getMinMaxAverageElevations();
  const double minElevation     = minMaxAverageElevations._x;
  const double maxElevation     = minMaxAverageElevations._y;
  const double deltaElevation   = maxElevation - minElevation;
  const double averageElevation = minMaxAverageElevations._z;

  ILogger::instance()->logInfo("Elevations: average=%f, min=%f max=%f delta=%f",
                               averageElevation, minElevation, maxElevation, deltaElevation);


//  FloatBufferBuilderFromGeodetic vertices(CenterStrategy::firstVertex(),
//                                          planet,
//                                          Vector3D::zero);
  FloatBufferBuilderFromGeodetic vertices = FloatBufferBuilderFromGeodetic::builderWithFirstVertexAsCenter(planet);
  FloatBufferBuilderFromColor colors;

  const Geodetic2D positionOffset2D = positionOffset.asGeodetic2D();

  for (int x = 0; x < _width; x++) {
    const double u = (double) x / (_width  - 1);

    for (int y = 0; y < _height; y++) {
      const double elevation = getElevationAt(x, y);
      if (ISNAN(elevation)) {
        continue;
      }

      const float alpha = (float) ((elevation - minElevation) / deltaElevation);
      const float r = alpha;
      const float g = alpha;
      const float b = alpha;
      colors.add(r, g, b, 1);

      const double v = 1.0 - ( (double) y / (_height - 1) );

      const Geodetic2D position = _sector.getInnerPoint(u, v).add(positionOffset2D);

      vertices.add(position,
                   positionOffset._height + (elevation * verticalExaggeration));

    }
  }

  const float lineWidth = 1;
  Color* flatColor = NULL;

  return new DirectMesh(GLPrimitive::points(),
                        //GLPrimitive::lineStrip(),
                        true,
                        vertices.getCenter(),
                        vertices.create(),
                        lineWidth,
                        pointSize,
                        flatColor,
                        colors.create(),
                        0,
                        false);
}

Interpolator* ElevationData::getInterpolator() const {
  if (_interpolator == NULL) {
    _interpolator = new BilinearInterpolator();
  }
  return _interpolator;
}

double ElevationData::getElevationAt(const Angle& latitude,
                                     const Angle& longitude) const {

  const IMathUtils* mu = IMathUtils::instance();

  const double nanD = mu->NanD();

  int _CHECK; // evaluate if this condiction can be rewrited using (uv < 0 || uv > 1)
  if (!_sector.contains(latitude, longitude)) {
    //    ILogger::instance()->logError("Sector %s doesn't contain lat=%s lon=%s",
    //                                  _sector.description().c_str(),
    //                                  latitude.description().c_str(),
    //                                  longitude.description().c_str());
    return nanD;
  }


  const Vector2D uv = _sector.getUVCoordinates(latitude, longitude);
  const double u = mu->clamp(uv._x, 0, 1);
  const double v = mu->clamp(uv._y, 0, 1);
  const double dX = u * (_width - 1);
  const double dY = (1.0 - v) * (_height - 1);

  const int x = (int) dX;
  const int y = (int) dY;
  const int nextX = x + 1;
  const int nextY = y + 1;
  const double alphaY = dY - y;
  const double alphaX = dX - x;

  double result;
  if (x == dX) {
    if (y == dY) {
      // exact on grid point
      result = getElevationAt(x, y);
    }
    else {
      // linear on Y
      const double heightY = getElevationAt(x, y);
      if (ISNAN(heightY)) {
        return nanD;
      }

      const double heightNextY = getElevationAt(x, nextY);
      if (ISNAN(heightNextY)) {
        return nanD;
      }

      result = mu->linearInterpolation(heightNextY, heightY, alphaY);
    }
  }
  else {
    if (y == dY) {
      // linear on X
      const double heightX = getElevationAt(x, y);
      if (ISNAN(heightX)) {
        return nanD;
      }
      const double heightNextX = getElevationAt(nextX, y);
      if (ISNAN(heightNextX)) {
        return nanD;
      }

      result = mu->linearInterpolation(heightX, heightNextX, alphaX);
    }
    else {
      // bilinear
      const double valueNW = getElevationAt(x, y);
      if (ISNAN(valueNW)) {
        return nanD;
      }
      const double valueNE = getElevationAt(nextX, y);
      if (ISNAN(valueNE)) {
        return nanD;
      }
      const double valueSE = getElevationAt(nextX, nextY);
      if (ISNAN(valueSE)) {
        return nanD;
      }
      const double valueSW = getElevationAt(x, nextY);
      if (ISNAN(valueSW)) {
        return nanD;
      }

      result = getInterpolator()->interpolation(valueSW,
                                                valueSE,
                                                valueNE,
                                                valueNW,
                                                alphaX,
                                                alphaY);
    }
  }
  
  return result;
}
