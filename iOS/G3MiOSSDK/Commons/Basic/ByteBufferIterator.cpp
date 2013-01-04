//
//  ByteBufferIterator.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 1/2/13.
//
//

#include "ByteBufferIterator.hpp"

#include "IByteBuffer.hpp"
#include "ILogger.hpp"
#include "ByteBufferBuilder.hpp"
#include "IMathUtils.hpp"

ByteBufferIterator::ByteBufferIterator(IByteBuffer* buffer) :
_buffer(buffer),
_cursor(0),
_timestamp(buffer->timestamp())
{

}

void ByteBufferIterator::checkTimestamp() const {
  if (_timestamp != _buffer->timestamp()) {
    ILogger::instance()->logError("The buffer was changed after the iteration started");
  }
}

bool ByteBufferIterator::hasNext() const {
  checkTimestamp();

  return ( _cursor < _buffer->size() );
}

unsigned char ByteBufferIterator::nextUInt8() {
  checkTimestamp();

  if (_cursor >= _buffer->size()) {
    ILogger::instance()->logError("Iteration overflow");
    return 0;
  }

  return _buffer->get(_cursor++);
}

int ByteBufferIterator::nextInt32() {
  // LittleEndian
  unsigned char b1 = nextUInt8();
  unsigned char b2 = nextUInt8();
  unsigned char b3 = nextUInt8();
  unsigned char b4 = nextUInt8();

  return ((unsigned int) b1) + (b2 << 8) + (b3 << 16) + (b4 << 24);
}

long long ByteBufferIterator::nextInt64() {
  // LittleEndian
  unsigned char b1 = nextUInt8();
  unsigned char b2 = nextUInt8();
  unsigned char b3 = nextUInt8();
  unsigned char b4 = nextUInt8();
  unsigned char b5 = nextUInt8();
  unsigned char b6 = nextUInt8();
  unsigned char b7 = nextUInt8();
  unsigned char b8 = nextUInt8();

  return ((long long) b1) + (b2 << 8) + (b3 << 16) + (b4 << 24) + ((long long) b5 << 32) + ((long long) b6 << 40) + ((long long) b7 << 48) + ((long long) b8 << 56);
}


IByteBuffer* ByteBufferIterator::nextBufferUpTo(unsigned char sentinel) {
  ByteBufferBuilder builder;

  unsigned char c;

  while ( (c = nextUInt8()) != sentinel ) {
    builder.add(c);
  }

  return builder.create();
}

const std::string ByteBufferIterator::nextZeroTerminatedString() {
  IByteBuffer* buffer = nextBufferUpTo((unsigned char) 0);
  const std::string result = buffer->getAsString();
  delete buffer;
  return result;
}

double ByteBufferIterator::nextDouble() {
  const long long l = nextInt64();
  return IMathUtils::instance()->rawLongBitsToDouble( l );
}