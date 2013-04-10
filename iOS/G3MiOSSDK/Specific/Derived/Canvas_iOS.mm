//
//  Canvas_iOS.mm
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 4/9/13.
//
//

#include "Canvas_iOS.hpp"

#include "G3MError.hpp"
#include "Color.hpp"
#include "Image_iOS.hpp"
#include "IImageListener.hpp"
#include "GFont.hpp"

Canvas_iOS::~Canvas_iOS() {
  if (_context) {
    CGContextRelease( _context );
    _context = NULL;
  }
}

void Canvas_iOS::tryToSetCurrentFontToContext() {
  if ((_currentUIFont != NULL) &&
      (_context != NULL)) {
    CGContextSelectFont(_context,
                        [[_currentUIFont fontName] UTF8String],
                        [_currentUIFont pointSize],
                        kCGEncodingMacRoman);
  }
}

void Canvas_iOS::_initialize(int width, int height) {
  CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();

  _context = CGBitmapContextCreate(NULL,       // memory created by Quartz
                                   width,
                                   height,
                                   8,          // bits per component
                                   width * 4,  // bitmap bytes per row: 4 bytes per pixel
                                   colorSpace,
                                   kCGImageAlphaPremultipliedLast);

  CGColorSpaceRelease( colorSpace );

  if (_context == NULL) {
    throw G3MError("Can't create CGContext");
  }

  tryToSetCurrentFontToContext();
}

CGColorRef Canvas_iOS::createCGColor(const Color& color) {
  return CGColorCreateCopy( [[UIColor colorWithRed: color.getRed()
                                             green: color.getGreen()
                                              blue: color.getBlue()
                                             alpha: color.getAlpha()] CGColor] );
}

void Canvas_iOS::_setFillColor(const Color& color) {
  CGContextSetRGBFillColor(_context,
                           color.getRed(),
                           color.getGreen(),
                           color.getBlue(),
                           color.getAlpha());
}

void Canvas_iOS::_setStrokeColor(const Color& color) {
  CGContextSetRGBStrokeColor(_context,
                             color.getRed(),
                             color.getGreen(),
                             color.getBlue(),
                             color.getAlpha());
}

void Canvas_iOS::_setStrokeWidth(float width) {
  CGContextSetLineWidth(_context, width);
}

void Canvas_iOS::_setShadow(const Color& color,
                            float blur,
                            float offsetX,
                            float offsetY) {
  CGColorRef cgColor = createCGColor(color);
  CGContextSetShadowWithColor(_context,
                              CGSizeMake(offsetX, offsetY),
                              blur,
                              cgColor);
  CGColorRelease(cgColor);
}

void Canvas_iOS::_removeShadow() {
  CGContextSetShadowWithColor(_context,
                              CGSizeMake(0, 0),
                              0,
                              NULL);
}

void Canvas_iOS::_createImage(IImageListener* listener,
                              bool autodelete) {
  CGImageRef cgImage = CGBitmapContextCreateImage(_context);
  UIImage* image = [UIImage imageWithCGImage: cgImage];
  CFRelease(cgImage);

  IImage* result = new Image_iOS(image, NULL);
  listener->imageCreated(result);
  if (autodelete) {
    delete listener;
  }
}

void Canvas_iOS::_fillRectangle(float x, float y,
                                float width, float height) {
  CGContextFillRect(_context,
                    CGRectMake(x, y,
                               width, height));
}


void Canvas_iOS::_strokeRectangle(float x, float y,
                                  float width, float height) {
  CGContextStrokeRect(_context,
                      CGRectMake(x, y,
                                 width, height));
}

void Canvas_iOS::drawRoundedRectangle(float x, float y,
                                      float width, float height,
                                      float radius,
                                      CGPathDrawingMode mode) {
  CGRect rrect = CGRectMake(x, y,
                            width, height);

	const float minx = CGRectGetMinX(rrect);
  const float midx = CGRectGetMidX(rrect);
  const float maxx = CGRectGetMaxX(rrect);
	const float miny = CGRectGetMinY(rrect);
  const float midy = CGRectGetMidY(rrect);
  const float maxy = CGRectGetMaxY(rrect);

	CGContextMoveToPoint(_context, minx, midy);
	CGContextAddArcToPoint(_context, minx, miny, midx, miny, radius);
	CGContextAddArcToPoint(_context, maxx, miny, maxx, midy, radius);
	CGContextAddArcToPoint(_context, maxx, maxy, midx, maxy, radius);
	CGContextAddArcToPoint(_context, minx, maxy, minx, midy, radius);
	CGContextClosePath(_context);
	CGContextDrawPath(_context, mode);
}

void Canvas_iOS::_fillRoundedRectangle(float x, float y,
                                       float width, float height,
                                       float radius) {
  drawRoundedRectangle(x, y,
                       width, height,
                       radius,
                       kCGPathFill);
}

void Canvas_iOS::_strokeRoundedRectangle(float x, float y,
                                         float width, float height,
                                         float radius) {
  drawRoundedRectangle(x, y,
                       width, height,
                       radius,
                       kCGPathStroke);
}

void Canvas_iOS::_fillAndStrokeRectangle(float x, float y,
                                         float width, float height) {
  _fillRectangle(x, y, width, height);
  _strokeRectangle(x, y, width, height);
}

void Canvas_iOS::_fillAndStrokeRoundedRectangle(float x, float y,
                                                float width, float height,
                                                float radius) {
  drawRoundedRectangle(x, y,
                       width, height,
                       radius,
                       kCGPathFillStroke);
}

UIFont* Canvas_iOS::createUIFont(const GFont& font) {
  const bool bold   = font.isBold();
  const bool italic = font.isItalic();

  NSString* fontName = nil;
  if (font.isSansSerif()) {
    if (bold) {
      if (italic) {
        fontName = @"Helvetica-BoldOblique";
      }
      else {
        fontName = @"Helvetica-Bold";
      }
    }
    else {
      if (italic) {
        fontName = @"Helvetica-Oblique";
      }
      else {
        fontName = @"Helvetica";
      }
    }
  }
  else if (font.isSerif()) {
    if (bold) {
      if (italic) {
        fontName = @"TimesNewRomanPS-BoldItalicMT";
      }
      else {
        fontName = @"TimesNewRomanPS-BoldMT";
      }
    }
    else {
      if (italic) {
        fontName = @"TimesNewRomanPS-ItalicMT";
      }
      else {
        fontName = @"TimesNewRomanPSMT";
      }
    }
  }
  else if (font.isMonospaced()) {
    if (bold) {
      if (italic) {
        fontName = @"Courier-BoldOblique";
      }
      else {
        fontName = @"Courier-Bold";
      }
    }
    else {
      if (italic) {
        fontName = @"Courier-Oblique";
      }
      else {
        fontName = @"Courier";
      }
    }
  }
  else {
    throw G3MError("Unsupported Font type");
  }

  return [UIFont fontWithName: fontName
                         size: font.getSize()];
}

const Vector2F Canvas_iOS::textExtent(const std::string& text,
                                      UIFont* uiFont) {
  NSString* nsString = [NSString stringWithCString: text.c_str()
                                          encoding: NSUTF8StringEncoding];

  CGSize cgSize = [nsString sizeWithFont: uiFont];

  return Vector2F(cgSize.width, cgSize.height);
}

const Vector2F Canvas_iOS::textExtent(const std::string& text,
                                      const GFont& font) {
  return textExtent(text, createUIFont(font));
}

void Canvas_iOS::_setFont(const GFont& font) {
  _currentUIFont = createUIFont(font);

  tryToSetCurrentFontToContext();
}

const Vector2F Canvas_iOS::_textExtent(const std::string& text) {
  return textExtent(text, _currentUIFont);
}

void Canvas_iOS::_fillText(const std::string& text,
                           float x, float y) {
  CGContextShowTextAtPoint(_context,
                           x, y,
                           text.c_str(),
                           text.size());
}