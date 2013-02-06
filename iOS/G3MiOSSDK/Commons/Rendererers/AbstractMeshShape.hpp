//
//  AbstractMeshShape.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 11/5/12.
//
//

#ifndef __G3MiOSSDK__AbstractMeshShape__
#define __G3MiOSSDK__AbstractMeshShape__

#include "Shape.hpp"
class Mesh;

class AbstractMeshShape : public Shape {
private:
  Mesh* _mesh;

protected:
  virtual Mesh* createMesh(const G3MRenderContext* rc) = 0;
  
  Mesh* getMesh(const G3MRenderContext* rc);

  void cleanMesh();

public:
  AbstractMeshShape(Geodetic3D* position) :
  Shape(position),
  _mesh(NULL) {

  }
  
  AbstractMeshShape(Geodetic3D* position,
                    Mesh* mesh) :
  Shape(position),
  _mesh(mesh) {

  }

  bool isReadyToRender(const G3MRenderContext* rc);

  void rawRender(const G3MRenderContext* rc,
                 const GLState& parentState);

  virtual ~AbstractMeshShape();

  bool isTransparent(const G3MRenderContext* rc);

};

#endif