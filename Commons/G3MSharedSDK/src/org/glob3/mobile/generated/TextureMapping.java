package org.glob3.mobile.generated; 
//
//  TextureMapping.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 12/07/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//

//
//  TextureMapping.hpp
//  G3MiOSSDK
//
//  Created by José Miguel S N on 12/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//


///#include <vector>

//class IGLTextureId;

//class G3MRenderContext;
//class IFloatBuffer;
//class GLGlobalState;
//class GPUProgramState;
//class GLState;
//class TexturesHandler;

public abstract class TextureMapping
{

  public void dispose()
  {
  }

  public abstract void modifyGLState(GLState state);
}