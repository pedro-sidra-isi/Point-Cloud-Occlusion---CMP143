#pragma once

#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "util.h"
#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <memory>
#define MAX_MATERIAL_COUNT 255
#define FLOATS_PER_TRI 9
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glm/vec3.hpp> // glm::vec3
#include <iostream>
#include <stdio.h>
#include <vector>

class Object {
public:
  Object(char const *FileName);
  Object(Object &&) = default;
  Object(const Object &) = delete;
  Object &operator=(Object &&) = default;
  Object &operator=(const Object &) = delete;
  ~Object();

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
