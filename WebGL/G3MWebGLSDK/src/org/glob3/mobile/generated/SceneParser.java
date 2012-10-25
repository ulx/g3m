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

	private static final String WMS110 = "1.1.0";
	private static final String WMS111 = "1.1.1";
	private static final String WMS130 = "1.3.0";

  private static SceneParser _instance = null;
  private java.util.HashMap<String, layer_type> mapLayerType = new java.util.HashMap<String, layer_type>();


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
  
	JSONObject json = IJSONParser.instance().parse(namelessParameter).getObject();
	parserJSONLayerList(layerSet, json.getObjectForKey(LAYERS).getObject());
	IJSONParser.instance().deleteJSONData(json);
  }

  private void parserJSONLayerList(LayerSet layerSet, JSONObject jsonLayers)
  {
	for (int i = 0; i < jsonLayers.getObject().getSize(); i++)
	{
	  IStringBuilder isb = IStringBuilder.newStringBuilder();
	  isb.addInt(i);
	  JSONObject jsonLayer = jsonLayers.getObjectForKey(isb.getString()).getObject();
	  final layer_type layerType = mapLayerType.get(jsonLayer.getObjectForKey(TYPE).getString().getValue());
  
	  switch (layerType)
	  {
		case WMS:
		  parserJSONWMSLayer(layerSet, jsonLayer);
		  break;
		case THREED:
		  parserJSON3DLayer(layerSet, jsonLayer);
		  break;
		case PANO:
		  parserJSONPanoLayer(layerSet, jsonLayer);
		  break;
	  }
  
	}
  }
  private void parserJSONWMSLayer(LayerSet layerSet, JSONObject jsonLayer)
  {
	System.out.print("Parsing WMS Layer ");
	System.out.print(jsonLayer.getObjectForKey(NAME).getString().getValue());
	System.out.print("...");
	System.out.print("\n");
  
	final String jsonDatasource = jsonLayer.getObjectForKey(DATASOURCE).getString().getValue();
	final int lastIndex = IStringUtils.instance().indexOf(jsonDatasource, "?");
	final String jsonURL = IStringUtils.instance().substring(jsonDatasource, 0, lastIndex+1);
	final String jsonVersion = jsonLayer.getObjectForKey(VERSION).getString().getValue();
  
	JSONArray jsonItems = jsonLayer.getObjectForKey(ITEMS).getArray();
	IStringBuilder layersName = IStringBuilder.newStringBuilder();
  
	for (int i = 0; i<jsonItems.getSize(); i++)
	{
		if (jsonItems.getElement(i).getObject().getObjectForKey(STATUS).getBoolean().getValue())
		{
		layersName.addString(jsonItems.getElement(i).getObject().getObjectForKey(NAME).getString().getValue());
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
	System.out.print(jsonLayer.getObjectForKey(NAME).getString().getValue());
	System.out.print("...");
	System.out.print("\n");
  }
  private void parserJSONPanoLayer(LayerSet layerSet, JSONObject jsonLayer)
  {
	System.out.print("Parsing Pano Layer ");
	System.out.print(jsonLayer.getObjectForKey(NAME).getString().getValue());
	System.out.print("...");
	System.out.print("\n");
  }

  protected SceneParser()
  {
	mapLayerType.put("WMS", layer_type.WMS);
	mapLayerType.put("THREED", layer_type.THREED);
	mapLayerType.put("PANO", layer_type.PANO);
  }
//C++ TO JAVA CONVERTER TODO TASK: The implementation of the following method could not be found:
//  SceneParser(SceneParser NamelessParameter);
//C++ TO JAVA CONVERTER TODO TASK: The implementation of the following method could not be found:
//  SceneParser operator = (SceneParser NamelessParameter);

}