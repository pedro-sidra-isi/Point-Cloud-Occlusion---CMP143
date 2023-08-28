#pragma once

#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "util.h"
#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glm/vec3.hpp> // glm::vec3
#include <iostream>
#include <memory>
#include <stdio.h>
#include <vector>

class PlyObject {
public:
  PlyObject(char const *FileName);
  PlyObject(PlyObject &&) = default;
  PlyObject(const PlyObject &) = delete;
  PlyObject &operator=(PlyObject &&) = default;
  PlyObject &operator=(const PlyObject &) = delete;
  ~PlyObject();

  GLuint numTriangles();
  GLuint numVertices();

  VertexArray VA;
  std::unique_ptr<VertexBuffer> VB;
  VertexBufferLayout layout;
  std::vector<Vertex> vertex_data;
  std::vector<Material> materials;

private:
  GLuint NumTris;

  void Load(char const *FileName);
};
