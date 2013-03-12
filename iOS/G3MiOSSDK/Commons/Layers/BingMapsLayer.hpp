//
//  BingMapsLayer.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 3/10/13.
//
//

#ifndef __G3MiOSSDK__BingMapsLayer__
#define __G3MiOSSDK__BingMapsLayer__

#include "Layer.hpp"

class BingMapsLayer : public Layer {
private:
  const std::string _imagerySet;
  const std::string _key;
  const Sector      _sector;
  const int         _initialLevel;

  bool _isInitialized;

  std::string _brandLogoUri;
  std::string _copyright;
  std::string _imageUrl;
  std::vector<std::string> _imageUrlSubdomains;

  void processMetadata(const std::string& brandLogoUri,
                       const std::string& copyright,
                       const std::string& imageUrl,
                       std::vector<std::string> imageUrlSubdomains,
                       const int imageWidth,
                       const int imageHeight,
                       const int zoomMin,
                       const int zoomMax);

  const std::string getQuadkey(const int level,
                               const int column,
                               const int row) const;

public:

  /**
   imagerySet: "Aerial", "AerialWithLabels", "Road", "OrdnanceSurvey" or "CollinsBart"
   key: Bing Maps key. See http://msdn.microsoft.com/en-us/library/gg650598.aspx
   */
  BingMapsLayer(const std::string& imagerySet,
                const std::string& key,
                const TimeInterval& timeToCache,
                int initialLevel = 1,
                LayerCondition* condition = NULL);

  URL getFeatureInfoURL(const Geodetic2D& position,
                        const Sector& sector) const;

  std::vector<Petition*> createTileMapPetitions(const G3MRenderContext* rc,
                                                const Tile* tile) const;
  
  bool isReady() const;

  void initialize(const G3MContext* context);

  void onDowloadMetadata(IByteBuffer* buffer);
  void onDownloadErrorMetadata();

};

#endif