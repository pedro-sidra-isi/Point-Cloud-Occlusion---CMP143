#pragma once

#include "IndexBuffer.h"
#include "PlyPointCloud.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "happly.h"
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
  PlyObject(char const *FileName, PlyPointCloud &p);
  ~PlyObject();

  GLuint numTriangles();
  GLuint numVertices();
  void update();

  VertexArray VA;
  std::shared_ptr<VertexBuffer> VB;
  VertexBufferLayout layout;
  std::vector<VertexWithResolution> vertex_data;
  std::vector<Material> materials;

  // typedef struct {
  //   float coordinate[4];
  //   float normal[3];
  //   uint material_index;
  //   uint point_count;
  //   float surface_area;
  //   float resolution;
  VertexWithResolution max_vertex_values;

private:
  GLuint NumTris;

  void Load(char const *FileName, PlyPointCloud &p);
};
