#pragma once

#include "IndexBuffer.h"
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

typedef struct {
  float coordinate[4];
} PCloudPoint;

class PlyPointCloud {
public:
  PlyPointCloud(char const *FileName);
  ~PlyPointCloud();

  GLuint numPoints();

  VertexArray VA;
  std::shared_ptr<VertexBuffer> VB;
  VertexBufferLayout layout;
  std::vector<PCloudPoint> point_data;

private:
  void Load(char const *FileName);
};
