package org.glob3.mobile.generated; 
public class SceneParser
{
	private static final String LAYERS = "layers";
	private static final String TYPE = "type";
	private static final String DATASOURCE = "datasource";
	private static final String VERSION = "version";
	private static final String ITEMS = "items";
	private static final String STATUS = "status";
	private static final String NAME = "name";
	private static final String URLICON = "urlIcon";
	private static final String MINDISTANCE = "minDistance";

	private static final String WMS110 = "1.1.0";
	private static final String WMS111 = "1.1.1";
	private static final String WMS130 = "1.3.0";

	private static SceneParser _instance = null;
	private java.util.HashMap<String, layer_type> _mapLayerType = new java.util.HashMap<String, layer_type>();
	private java.util.HashMap<String, java.util.HashMap<String, String> > _mapGeoJSONSources = new java.util.HashMap<String, java.util.HashMap<String, String> >();
	private java.util.ArrayList<String> _panoSources = new java.util.ArrayList<String>();


	public static SceneParser instance()
	{
		if (_instance == null)
		{
			_instance = new SceneParser();
		}
		return _instance;
	}
	public final void parse(LayerSet layerSet, String namelessParameter)
	{
    
		_mapGeoJSONSources.clear();
		_panoSources.clear();
    
		JSONBaseObject json = IJSONParser.instance().parse(namelessParameter);
		parserJSONLayerList(layerSet, json.asObject().getAsObject(LAYERS));
		IJSONParser.instance().deleteJSONData(json);
	}
	public final java.util.HashMap<String, java.util.HashMap<String, String> > getMapGeoJSONSources()
	{
		return _mapGeoJSONSources;
	}
	public final java.util.ArrayList<String> getPanoSources()
	{
		return _panoSources;
	}

//C++ TO JAVA CONVERTER TODO TASK: The implementation of the following method could not be found:
//	void parserJSONLayerList(LayerSet layerSet, JSONObject jsonLayers);
	private void parserJSONWMSLayer(LayerSet layerSet, JSONObject jsonLayer)
	{
		System.out.print("Parsing WMS Layer ");
		System.out.print(jsonLayer.getAsString(NAME).value());
		System.out.print("...");
		System.out.print("\n");
    
		final String jsonDatasource = jsonLayer.getAsString(DATASOURCE).value();
		final int lastIndex = IStringUtils.instance().indexOf(jsonDatasource, "?");
		final String jsonURL = IStringUtils.instance().substring(jsonDatasource, 0, lastIndex+1);
		final String jsonVersion = jsonLayer.getAsString(VERSION).value();
    
		final JSONArray jsonItems = jsonLayer.getAsArray(ITEMS);
		IStringBuilder layersName = IStringBuilder.newStringBuilder();
    
		for (int i = 0; i<jsonItems.size(); i++)
		{
			if (jsonItems.getAsObject(i).getAsBoolean(STATUS).value())
			{
				layersName.addString(jsonItems.getAsObject(i).getAsString(NAME).value());
				layersName.addString(",");
			}
		}
		String layersSecuence = layersName.getString();
		if (layersName.getString().length() > 0)
		{
			layersSecuence = IStringUtils.instance().substring(layersSecuence, 0, layersSecuence.length()-1);
		}
    
		//TODO check if wms 1.1.1 is neccessary to have it in account
		WMSServerVersion wmsVersion = WMSServerVersion.WMS_1_1_0;
		if (jsonVersion.compareTo(WMS130)==0)
		{
			wmsVersion = WMSServerVersion.WMS_1_3_0;
		}
    
		WMSLayer wmsLayer = new WMSLayer(URL.escape(layersSecuence), new URL(jsonURL, false), wmsVersion, Sector.fullSphere(), "image/png", "EPSG:4326", "", true, null);
		layerSet.addLayer(wmsLayer);
	}
	private void parserJSON3DLayer(LayerSet layerSet, JSONObject jsonLayer)
	{
		System.out.print("Parsing 3D Layer ");
		System.out.print(jsonLayer.getAsString(NAME).value());
		System.out.print("...");
		System.out.print("\n");
	}
	private void parserJSONPlanarImageLayer(LayerSet layerSet, JSONObject jsonLayer)
	{
		System.out.print("Parsing Pano Layer ");
		System.out.print(jsonLayer.getAsString(NAME).value());
		System.out.print("...");
		System.out.print("\n");
    
		final String geojsonDatasource = jsonLayer.getAsString(DATASOURCE).value();
    
		final JSONArray jsonItems = jsonLayer.getAsArray(ITEMS);
		for (int i = 0; i<jsonItems.size(); i++)
		{
    
			final String namefile = jsonItems.getAsObject(i).getAsString(NAME).value();
    
			IStringBuilder url = IStringBuilder.newStringBuilder();
			url.addString(geojsonDatasource);
			url.addString("/");
			url.addString(namefile);
    
			_panoSources.add(url.getString());
		}
    
    
	}
	private void parserJSONSphericalImageLayer(LayerSet layerSet, JSONObject jsonLayer)
	{
		System.out.print("Parsing GEOJSON Layer not available");
		System.out.print("\n");
	}
	private void parserGEOJSONLayer(LayerSet layerSet, JSONObject jsonLayer)
	{
		System.out.print("Parsing GEOJSON Layer ");
		System.out.print(jsonLayer.getAsString(NAME).value());
		System.out.print("...");
		System.out.print("\n");
    
		final String geojsonDatasource = jsonLayer.getAsString(DATASOURCE).value();
    
		final JSONArray jsonItems = jsonLayer.getAsArray(ITEMS);
		for (int i = 0; i<jsonItems.size(); i++)
		{
    
			final String namefile = jsonItems.getAsObject(i).getAsString(NAME).value();
			final String icon = jsonItems.getAsObject(i).getAsString(URLICON).value();
			final String minDistance = jsonItems.getAsObject(i).getAsString(MINDISTANCE).value();
    
    
			IStringBuilder url = IStringBuilder.newStringBuilder();
			url.addString(geojsonDatasource);
			url.addString("/");
			url.addString(namefile);
    
			java.util.HashMap<String, String> attr = new java.util.HashMap<String, String>();
			attr.put(URLICON, icon);
			attr.put(MINDISTANCE, minDistance);
    
			_mapGeoJSONSources.put(url.getString(), attr);
		}
	}

	protected SceneParser()
	{
		_mapLayerType.put("WMS", layer_type.WMS);
		_mapLayerType.put("THREED", layer_type.THREED);
		_mapLayerType.put("PLANARIMAGE", layer_type.PLANARIMAGE);
		_mapLayerType.put("GEOJSON", layer_type.GEOJSON);
		_mapLayerType.put("SPHERICALIMAGE", layer_type.SPHERICALIMAGE);
    
	}
//C++ TO JAVA CONVERTER TODO TASK: The implementation of the following method could not be found:
//	SceneParser(SceneParser NamelessParameter);
//C++ TO JAVA CONVERTER TODO TASK: The implementation of the following method could not be found:
//	SceneParser operator = (SceneParser NamelessParameter);

}