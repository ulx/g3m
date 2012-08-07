package org.glob3.mobile.generated; 
//
//  TextureBuilder.hpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 23/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//



public abstract class TextureBuilder
{
//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: virtual int createTextureFromImages(GL * gl, const java.util.ArrayList<const IImage*>& vImages, int width, int height) const = 0;
  public abstract int createTextureFromImages(GL gl, java.util.ArrayList<IImage> vImages, int width, int height);

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: virtual int createTextureFromImages(GL * gl, const IFactory* factory, const java.util.ArrayList<const IImage*>& vImages, const java.util.ArrayList<const Rectangle*>& vRectangles, int width, int height) const = 0;
  public abstract int createTextureFromImages(GL gl, IFactory factory, java.util.ArrayList<IImage> vImages, java.util.ArrayList<Rectangle> vRectangles, int width, int height);


  public void dispose()
  {
  }
}