//
//  MercatorTiledLayer.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 3/8/13.
//
//

#include "MercatorTiledLayer.hpp"

#include "Vector2I.hpp"
#include "LayerTilesRenderParameters.hpp"
#include "Tile.hpp"
#include "IStringBuilder.hpp"
#include "Petition.hpp"

/*

 Implementation details:

 - - from http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
 - - the G3M level 0 is mapped to initialMercatorLevel
 - - so maxLevel is maxMercatorLevel-initialOSMLevel
 - - and splitsByLatitude and splitsByLongitude are set to 2^initialMercatorLevel

 */

MercatorTiledLayer::MercatorTiledLayer(const std::string& name,
                                       const std::string& protocol,
                                       const std::string& domain,
                                       const std::vector<std::string>& subdomains,
                                       const std::string&              imageFormat,
                                       const TimeInterval& timeToCache,
                                       const Sector sector,
                                       int initialMercatorLevel,
                                       int maxMercatorLevel,
                                       LayerCondition* condition) :
Layer(condition,
      name,
      timeToCache,
      new LayerTilesRenderParameters(Sector::fullSphere(),
                                     (int) IMathUtils::instance()->pow(2.0, initialMercatorLevel),
                                     (int) IMathUtils::instance()->pow(2.0, initialMercatorLevel),
                                     maxMercatorLevel - initialMercatorLevel,
                                     Vector2I(256, 256),
                                     LayerTilesRenderParameters::defaultTileMeshResolution(),
                                     true)),
_protocol(protocol),
_domain(domain),
_subdomains(subdomains),
_imageFormat(imageFormat),
_sector(sector),
_initialMercatorLevel(initialMercatorLevel)
{

}

URL MercatorTiledLayer::getFeatureInfoURL(const Geodetic2D& position,
                                          const Sector& sector) const {
  return URL();
}

std::vector<Petition*> MercatorTiledLayer::createTileMapPetitions(const G3MRenderContext* rc,
                                                                  const Tile* tile) const {
  std::vector<Petition*> petitions;

  const Sector tileSector = tile->getSector();
  if (!_sector.touchesWith(tileSector)) {
    return petitions;
  }

  const Sector sector = tileSector.intersection(_sector);
  if (sector.getDeltaLatitude().isZero() ||
      sector.getDeltaLongitude().isZero() ) {
    return petitions;
  }

  // http://[abc].tile.openstreetmap.org/zoom/x/y.png
  // http://[abc].tiles.mapbox.com/v3/examples.map-vyofok3q/9/250/193.png

  const int level   = tile->getLevel() + _initialMercatorLevel;
  const int column  = tile->getColumn();
  const int numRows = (int) IMathUtils::instance()->pow(2.0, level);
  const int row     = numRows - tile->getRow() - 1;

  IStringBuilder* isb = IStringBuilder::newStringBuilder();

  isb->addString(_protocol);

  const int subdomainsSize = _subdomains.size();
  if (subdomainsSize > 0) {
    // select subdomain based on fixed data (instead of round-robin) to be cache friendly
    const int subdomainsIndex =  IMathUtils::instance()->abs(level + column + row) % subdomainsSize;
#ifdef C_CODE
    isb->addString(_subdomains[subdomainsIndex]);
#endif
#ifdef JAVA_CODE
    isb.addString(_subdomains.get(subdomainsIndex));
#endif
  }

  isb->addString(_domain);
  isb->addString("/");

  isb->addInt(level);
  isb->addString("/");

  isb->addInt(column);
  isb->addString("/");

  isb->addInt(row);
  isb->addString(".");
  isb->addString(_imageFormat);

  const std::string path = isb->getString();

  delete isb;

  petitions.push_back( new Petition(tileSector,
                                    URL(path, false),
                                    _timeToCache,
                                    true) );
  
  return petitions;
}