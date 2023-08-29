#pragma once

#include <GL3/gl3w.h>

#include "Debug.h"
#include "FullImageBuffer.hpp"
#include "IndexBuffer.h"
#include "Object.h"
#include "PlyPointCloud.h"
#include "Shader.h"
#include "VertexArray.h"
#include "opengl_reference.h"

class Renderer {
public:
  Renderer();
  void Clear() const;
  void Draw(const VertexArray &va, const Shader &shader, unsigned int numVerts,
            GLuint type = GL_TRIANGLES) const;
  void Draw(const Object &object, const Shader &shader,
            GLuint type = GL_TRIANGLES) const;
  void Draw(const PlyObject &object, const Shader &shader,
            GLuint type = GL_TRIANGLES) const;
  void Draw(const PlyPointCloud &object, const Shader &shader) const;

  void DrawFullscreenImage(FullImageBuffer &src, Shader &shader,
                           unsigned char *image);
};
