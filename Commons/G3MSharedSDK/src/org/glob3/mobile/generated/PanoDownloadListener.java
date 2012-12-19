package org.glob3.mobile.generated; 
//
//  PanoDownloadListener.cpp
//  G3MiOSSDK
//
//  Created by Eduardo de la Montaña on 7/12/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//


//
//  PanoDownloadListener.hpp
//  G3MiOSSDK
//
//  Created by Eduardo de la Montaña on 7/12/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//



//C++ TO JAVA CONVERTER NOTE: Java has no need of forward class declarations:
//class MarksRenderer;
public class PanoDownloadListener implements IBufferDownloadListener
{

	private static final String NAME = "name";
	private static final String POSITION = "position";
	private static final String LAT = "lat";
	private static final String LON = "lon";

	private MarksRenderer _marksRenderer;
	private MarkTouchListener _panoTouchListener;

	public PanoDownloadListener(MarksRenderer marksRenderer, MarkTouchListener panoTouchListener)
	{
		_marksRenderer = marksRenderer;
		_panoTouchListener = panoTouchListener;
	}

	public final void onDownload(URL url, IByteBuffer buffer)
	{
		String String = buffer.getAsString();
		JSONBaseObject json = IJSONParser.instance().parse(String);
		ILogger.instance().logInfo(url.getPath());
		parseMETADATA(IStringUtils.instance().substring(url.getPath(), 0, IStringUtils.instance().indexOf(url.getPath(), "/metadata.json")), json.asObject());
		IJSONParser.instance().deleteJSONData(json);
	}

	public final void onError(URL url)
	{
		ILogger.instance().logError("The requested pano could not be found!");
	}

	public final void onCancel(URL url)
	{
	}
	public final void onCanceledDownload(URL url, IByteBuffer data)
	{
	}

	public void dispose()
	{
	}
	private void parseMETADATA(String url, JSONObject json)
	{
    
		final Angle latitude = Angle.fromDegrees(json.getAsObject(POSITION).getAsNumber(LAT).doubleValue());
		final Angle longitude = Angle.fromDegrees(json.getAsObject(POSITION).getAsNumber(LON).doubleValue());
    
		Mark mark = new Mark(json.getAsString(NAME).value(), new URL("http://glob3m.glob3mobile.com/icons/markers/g3m.png",false), new Geodetic3D(latitude, longitude, 0), new URL(url, false), 0, _panoTouchListener);
    
		_marksRenderer.addMark(mark);
	}
}