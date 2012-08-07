package org.glob3.mobile.generated; 
//
//  MutableVector3D.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 31/05/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//

//
//  MutableVector3D.hpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 31/05/12.
//  Copyright (c) 2012 IGO Software SL. All rights reserved.
//




//class Vector3D;

public class MutableVector3D
{
  private double _x;
  private double _y;
  private double _z;


  public MutableVector3D()
  {
	  _x = 0;
	  _y = 0;
	  _z = 0;
  }

  public MutableVector3D(double x, double y, double z)
  {
	  _x = x;
	  _y = y;
	  _z = z;

  }

  public MutableVector3D(MutableVector3D v)
  {
	  _x = v._x;
	  _y = v._y;
	  _z = v._z;

  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: MutableVector3D normalized() const
  public final MutableVector3D normalized()
  {
	double d = length();
	return new MutableVector3D(_x / d, _y /d, _z / d);
  }

  public static MutableVector3D nan()
  {
	return new MutableVector3D(Double.NaN, Double.NaN, Double.NaN);
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: boolean isNan() const
  public final boolean isNan()
  {
	return (Double.isNaN(_x) || Double.isNaN(_y) || Double.isNaN(_z));
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: boolean isZero() const
  public final boolean isZero()
  {
	return (_x == 0) && (_y == 0) && (_z == 0);
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: void print() const
  public final void print()
  {
	System.out.printf("%.2f  %.2f %.2f\n", _x, _y, _z);
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: double length() const
  public final double length()
  {
	return Math.sqrt(squaredLength());
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: double squaredLength() const
  public final double squaredLength()
  {
	return _x * _x + _y * _y + _z * _z;
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: double dot(const MutableVector3D& v) const
  public final double dot(MutableVector3D v)
  {
	return _x * v._x + _y * v._y + _z * v._z;
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: MutableVector3D add(const MutableVector3D& v) const
  public final MutableVector3D add(MutableVector3D v)
  {
	return new MutableVector3D(_x + v._x, _y + v._y, _z + v._z);
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: MutableVector3D sub(const MutableVector3D& v) const
  public final MutableVector3D sub(MutableVector3D v)
  {
	return new MutableVector3D(_x - v._x, _y - v._y, _z - v._z);
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: MutableVector3D times(const MutableVector3D& v) const
  public final MutableVector3D times(MutableVector3D v)
  {
	return new MutableVector3D(_x * v._x, _y * v._y, _z * v._z);
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: MutableVector3D times(const double magnitude) const
  public final MutableVector3D times(double magnitude)
  {
	return new MutableVector3D(_x * magnitude, _y * magnitude, _z * magnitude);
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: MutableVector3D div(const MutableVector3D& v) const
  public final MutableVector3D div(MutableVector3D v)
  {
	return new MutableVector3D(_x / v._x, _y / v._y, _z / v._z);
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: MutableVector3D div(const double v) const
  public final MutableVector3D div(double v)
  {
	return new MutableVector3D(_x / v, _y / v, _z / v);
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: MutableVector3D cross(const MutableVector3D& other) const
  public final MutableVector3D cross(MutableVector3D other)
  {
	return new MutableVector3D(_y * other._z - _z * other._y, _z * other._x - _x * other._z, _x * other._y - _y * other._x);
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: Angle angleBetween(const MutableVector3D& other) const
  public final Angle angleBetween(MutableVector3D other)
  {
	final MutableVector3D v1 = normalized();
	final MutableVector3D v2 = other.normalized();
  
	double c = v1.dot(v2);
	if (c > 1.0)
		c = 1.0;
	else if (c < -1.0)
		c = -1.0;
  
	return Angle.fromRadians(Math.acos(c));
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: MutableVector3D rotatedAroundAxis(const MutableVector3D& axis, const Angle& theta) const
  public final MutableVector3D rotatedAroundAxis(MutableVector3D axis, Angle theta)
  {
	final double u = axis.x();
	final double v = axis.y();
	final double w = axis.z();
  
	final double cosTheta = theta.cosinus();
	final double sinTheta = theta.sinus();
  
	final double ms = axis.squaredLength();
	final double m = Math.sqrt(ms);
  
	return new MutableVector3D(((u * (u * _x + v * _y + w * _z)) + (((_x * (v * v + w * w)) - (u * (v * _y + w * _z))) * cosTheta) + (m * ((-w * _y) + (v * _z)) * sinTheta)) / ms, ((v * (u * _x + v * _y + w * _z)) + (((_y * (u * u + w * w)) - (v * (u * _x + w * _z))) * cosTheta) + (m * ((w * _x) - (u * _z)) * sinTheta)) / ms, ((w * (u * _x + v * _y + w * _z)) + (((_z * (u * u + v * v)) - (w * (u * _x + v * _y))) * cosTheta) + (m * (-(v * _x) + (u * _y)) * sinTheta)) / ms);
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: double x() const
  public final double x()
  {
	return _x;
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: double y() const
  public final double y()
  {
	return _y;
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: double z() const
  public final double z()
  {
	return _z;
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: MutableVector3D transformedBy(const MutableMatrix44D &m, const double homogeneus) const
  public final MutableVector3D transformedBy(MutableMatrix44D m, double homogeneus)
  {
	return new MutableVector3D(_x * m.get(0) + _y * m.get(4) + _z * m.get(8) + homogeneus * m.get(12), _x * m.get(1) + _y * m.get(5) + _z * m.get(9) + homogeneus * m.get(13), _x * m.get(2) + _y * m.get(6) + _z * m.get(10) + homogeneus * m.get(14));
  }

//C++ TO JAVA CONVERTER WARNING: 'const' methods are not available in Java:
//ORIGINAL LINE: Vector3D asVector3D() const
  public final Vector3D asVector3D()
  {
	return new Vector3D(_x, _y, _z);
  }

}