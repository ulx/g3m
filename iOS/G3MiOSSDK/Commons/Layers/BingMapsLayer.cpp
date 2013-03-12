//
//  BingMapsLayer.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 3/10/13.
//
//

#include "BingMapsLayer.hpp"

#include "Vector2I.hpp"
#include "LayerTilesRenderParameters.hpp"
#include "Tile.hpp"
#include "IStringBuilder.hpp"
#include "IStringUtils.hpp"
#include "Petition.hpp"
#include "IDownloader.hpp"
#include "DownloadPriority.hpp"
#include "IBufferDownloadListener.hpp"
#include "IJSONParser.hpp"
#include "JSONObject.hpp"
#include "JSONArray.hpp"
#include "JSONNumber.hpp"

BingMapsLayer::BingMapsLayer(const std::string& imagerySet,
                             const std::string& key,
                             const TimeInterval& timeToCache,
                             int initialLevel,
                             LayerCondition* condition) :
Layer(condition,
      "BingMaps",
      timeToCache,
      NULL),
_imagerySet(imagerySet),
_key(key),
_initialLevel(initialLevel),
_sector(Sector::fullSphere()),
_isInitialized(false)
{

}


class BingMapsLayer_MetadataBufferDownloadListener : public IBufferDownloadListener {
private:
  BingMapsLayer* _bingMapsLayer;
  
public:
  BingMapsLayer_MetadataBufferDownloadListener(BingMapsLayer* bingMapsLayer) :
  _bingMapsLayer(bingMapsLayer)
  {

  }

  void onDownload(const URL& url,
                  IByteBuffer* buffer) {
    _bingMapsLayer->onDowloadMetadata(buffer);
  }

  void onError(const URL& url) {
    _bingMapsLayer->onDownloadErrorMetadata();
  }

  void onCancel(const URL& url) {
    // do nothing, the request won't be cancelled
  }

  void onCanceledDownload(const URL& url,
                          IByteBuffer* data) {
    // do nothing, the request won't be cancelled
  }
};


void BingMapsLayer::onDownloadErrorMetadata() {
  ILogger::instance()->logError("BingMapsLayer: Error while downloading metadata.");
}

bool BingMapsLayer::isReady() const {
  return _isInitialized;
}

void BingMapsLayer::onDowloadMetadata(IByteBuffer* buffer) {
  IJSONParser* parser = IJSONParser::instance();

  const JSONBaseObject* jsonBaseObject = parser->parse(buffer);
  if (jsonBaseObject == NULL) {
    ILogger::instance()->logError("BingMapsLayer: Can't parse json metadata.");
    return;
  }

  const JSONObject* jsonObject = jsonBaseObject->asObject();
  if (jsonObject == NULL) {
    ILogger::instance()->logError("BingMapsLayer: Error while parsing json metadata, root object is not an json-object.");
    parser->deleteJSONData(jsonBaseObject);
    return;
  }

  const std::string brandLogoUri = jsonObject->getAsString("brandLogoUri", "");
  const std::string copyright    = jsonObject->getAsString("copyright",    "");

  const JSONArray* resourceSets = jsonObject->getAsArray("resourceSets");
  if (resourceSets == NULL) {
    ILogger::instance()->logError("BingMapsLayer: Error while parsing json metadata, resourceSets field not found.");
    parser->deleteJSONData(jsonBaseObject);
    return;
  }

  if (resourceSets->size() != 1) {
    ILogger::instance()->logError("BingMapsLayer: Error while parsing json metadata, resourceSets has %d elements (the current implementation can only handle 1 element).",
                                  resourceSets->size());
    parser->deleteJSONData(jsonBaseObject);
    return;
  }

  const JSONObject* resource = resourceSets->getAsObject(0);
  if (resource == NULL) {
    ILogger::instance()->logError("BingMapsLayer: Error while parsing json metadata, can't find resource jsonobject.");
    parser->deleteJSONData(jsonBaseObject);
    return;
  }

  const JSONArray* resources = resource->getAsArray("resources");
  if (resources->size() != 1) {
    ILogger::instance()->logError("BingMapsLayer: Error while parsing json metadata, resources has %d elements (the current implementation can only handle 1 element).",
                                  resources->size());
    parser->deleteJSONData(jsonBaseObject);
    return;
  }

  const JSONObject* meanfulResource = resources->getAsObject(0);
  if (meanfulResource == NULL) {
    ILogger::instance()->logError("BingMapsLayer: Error while parsing json metadata, can't find a meanfulResource JSONObject.");
    parser->deleteJSONData(jsonBaseObject);
    return;
  }

  const std::string imageUrl = meanfulResource->getAsString("imageUrl", "");
  if (imageUrl.size() == 0) {
    ILogger::instance()->logError("BingMapsLayer: Error while parsing json metadata, can't find a imageUrl String.");
    parser->deleteJSONData(jsonBaseObject);
    return;
  }

  const int imageWidth  = (int) meanfulResource->getAsNumber("imageWidth",  256);
  const int imageHeight = (int) meanfulResource->getAsNumber("imageHeight", 256);

  const int zoomMin = (int) meanfulResource->getAsNumber("zoomMin", _initialLevel+1);
  const int zoomMax = (int) meanfulResource->getAsNumber("zoomMax", _initialLevel+1);

  const JSONArray* imageUrlSubdomainsJS = meanfulResource->getAsArray("imageUrlSubdomains");
  if (imageUrlSubdomainsJS == NULL) {
    ILogger::instance()->logError("BingMapsLayer: Error while parsing json metadata, can't find a imageUrlSubdomains JSONArray.");
    parser->deleteJSONData(jsonBaseObject);
    return;
  }

  std::vector<std::string> imageUrlSubdomains;
  for (int i = 0; i < imageUrlSubdomainsJS->size(); i++) {
    const std::string imageUrlSubdomain = imageUrlSubdomainsJS->getAsString(i, "");
    if (imageUrlSubdomain.size() != 0) {
      imageUrlSubdomains.push_back(imageUrlSubdomain);
    }
  }

  if (imageUrlSubdomains.size() == 0) {
    ILogger::instance()->logError("BingMapsLayer: Error while parsing json metadata, can't find any imageUrlSubdomain String.");
    parser->deleteJSONData(jsonBaseObject);
    return;
  }

  processMetadata(brandLogoUri,
                  copyright,
                  imageUrl,
                  imageUrlSubdomains,
                  imageWidth, imageHeight,
                  zoomMin, zoomMax);

//  http://ecn.{subdomain}.tiles.virtualearth.net/tiles/h{quadkey}.jpeg?g=1180&mkt={culture}


  parser->deleteJSONData(jsonBaseObject);
  delete buffer;
}

void BingMapsLayer::processMetadata(const std::string& brandLogoUri,
                                    const std::string& copyright,
                                    const std::string& imageUrl,
                                    std::vector<std::string> imageUrlSubdomains,
                                    const int imageWidth,
                                    const int imageHeight,
                                    const int zoomMin,
                                    const int zoomMax) {
  _brandLogoUri = brandLogoUri;
  _copyright = copyright;
  _imageUrl = imageUrl;
  _imageUrlSubdomains = imageUrlSubdomains;

  _isInitialized = true;

  setParameters(new LayerTilesRenderParameters(Sector::fullSphere(),
                                               (int) IMathUtils::instance()->pow(2.0, _initialLevel),
                                               (int) IMathUtils::instance()->pow(2.0, _initialLevel),
                                               zoomMax - _initialLevel,
                                               Vector2I(imageWidth, imageHeight),
                                               LayerTilesRenderParameters::defaultTileMeshResolution(),
                                               true));
}

void BingMapsLayer::initialize(const G3MContext* context) {
  const URL url("http://dev.virtualearth.net/REST/v1/Imagery/Metadata/" + _imagerySet + "?key=" + _key, false);

  context->getDownloader()->requestBuffer(url,
                                          DownloadPriority::HIGHEST,
                                          TimeInterval::fromDays(1),
                                          new BingMapsLayer_MetadataBufferDownloadListener(this),
                                          true);
}

URL BingMapsLayer::getFeatureInfoURL(const Geodetic2D& position,
                                     const Sector& sector) const {
  return URL();
}

const std::string BingMapsLayer::getQuadkey(const int zoom,
                                            const int column,
                                            const int row) const {
  IStringBuilder* isb = IStringBuilder::newStringBuilder();

  for (int i = 1; i <= zoom; i++) {
    const int t = (((row >> zoom - i) & 1) << 1) | ((column >> zoom - i) & 1);
    isb->addInt(t);
  }

  const std::string result = isb->getString();

  delete isb;

  return result;
}

std::vector<Petition*> BingMapsLayer::createTileMapPetitions(const G3MRenderContext* rc,
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

  const IStringUtils* su = IStringUtils::instance();
  
  const int level   = tile->getLevel() + _initialLevel;
  const int column  = tile->getColumn();
  const int numRows = (int) IMathUtils::instance()->pow(2.0, level);
  const int row     = numRows - tile->getRow() - 1;

  const int subdomainsSize = _imageUrlSubdomains.size();
  std::string subdomain = "";
  if (subdomainsSize > 0) {
    // select subdomain based on fixed data (instead of round-robin) to be cache friendly
    const int subdomainsIndex =  IMathUtils::instance()->abs(level + column + row) % subdomainsSize;
//#ifdef C_CODE
    subdomain = _imageUrlSubdomains[subdomainsIndex];
//#endif
//#ifdef JAVA_CODE
//    subdomain = _imageUrlSubdomains.get(subdomainsIndex);
//#endif
  }

  const std::string quadkey = getQuadkey(level, column, row);

  std::string path = _imageUrl;
  path = su->replaceSubstring(path, "{subdomain}", subdomain);
  path = su->replaceSubstring(path, "{quadkey}",   quadkey);
  path = su->replaceSubstring(path, "{culture}",   "en-US");

  petitions.push_back( new Petition(tileSector,
                                    URL(path, false),
                                    _timeToCache,
                                    true) );
  
  return petitions;
}