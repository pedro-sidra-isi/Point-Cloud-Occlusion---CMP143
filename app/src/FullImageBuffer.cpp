#include "FullImageBuffer.hpp"

void FullImageBuffer::draw(unsigned char *image) {
  // GLCall(glFrontFace(GL_CW));
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0,
  //              GL_RGBA, GL_UNSIGNED_BYTE, image);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  GLCall(glDisable(GL_CULL_FACE));

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, image);
  // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
  // GL_RGB,
  //                 GL_UNSIGNED_BYTE, image);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, imageTexture);
}

FullImageBuffer::~FullImageBuffer() {}
FullImageBuffer::FullImageBuffer() {

  //  Define the vertices of 2 CCW triangles (quad) in the normalized device
  //  coordinates [‐1,1]x[‐1,1] covering the entire window, and their
  //  corresponding texture coordinates. The result of the rasterized image
  //  produced by Close2GL will be displayed by texture mapping the triangles.

  //                     x      y      s     t
  vertex_data = new float[4 * 6];
  float standard_vertex_data[] = {-1.0f, -1.0f, 0.0f, 0.0f,  // v0
                                  1.0f,  -1.0f, 1.0f, 0.0f,  // v1
                                  1.0f,  1.0f,  1.0f, 1.0f,  // 1st triangle
                                  1.0f,  1.0f,  1.0f, 1.0f,  // v0
                                  -1.0f, 1.0f,  0.0f, 1.0f,  // v1
                                  -1.0f, -1.0f, 0.0f, 0.0f}; // 2nd triangle
  memcpy(vertex_data, standard_vertex_data, sizeof(standard_vertex_data));

  VB = std::make_shared<VertexBuffer>(vertex_data, 4 * 6 * sizeof(float), 6);
  layout.AddFloat(2);
  layout.AddFloat(2);
  VA.AddBuffer(*VB, layout);

  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &imageTexture);
  glBindTexture(GL_TEXTURE_2D, imageTexture);
  // glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT);
}
