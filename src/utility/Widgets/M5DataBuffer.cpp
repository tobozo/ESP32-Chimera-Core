#include "M5DataBuffer.h"

DataBuffer::DataBuffer(const void* data, int dataSize) {
  this->data = NULL;
  set(data, dataSize);
}

DataBuffer::~DataBuffer() {
  free(data);
}

void DataBuffer::growToFit(int newDataSize) {
  if (this->dataSize >= newDataSize) return;
  this->dataSize = newDataSize;
  this->data = realloc(this->data, this->dataSize);
}

void DataBuffer::resize(int newDataSize) {
  if (this->dataSize == newDataSize) return;
  this->dataSize = newDataSize;
  this->data = realloc(this->data, this->dataSize);
}

void DataBuffer::set(const void* data, int dataSize) {
  free(this->data);
  this->dataSize = dataSize;
  this->data = malloc(dataSize);
  memcpy(this->data, data, dataSize);
}

void DataBuffer::setTextRaw(const char* text, int textLen) {
  set(text, textLen+1);
}

DataBuffer& DataBuffer::operator=( const DataBuffer& other ) {
  set(other.data, other.dataSize);
  return *this;
}