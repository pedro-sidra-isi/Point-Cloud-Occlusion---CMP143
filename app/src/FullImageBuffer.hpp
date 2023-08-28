#pragma once
#include "VertexArray.h"
#include "util.h"

class FullImageBuffer {
public:
  FullImageBuffer();
  FullImageBuffer(FullImageBuffer &&) = default;
  FullImageBuffer(const FullImageBuffer &) = default;
  FullImageBuffer &operator=(FullImageBuffer &&) = default;
  FullImageBuffer &operator=(const FullImageBuffer &) = default;
  ~FullImageBuffer();

  void draw(unsigned char *image);

  float *vertex_data;
  VertexArray VA;
  std::shared_ptr<VertexBuffer> VB;

  GLuint imageTexture;

private:
  VertexBufferLayout layout;
};
