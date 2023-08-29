#include "PlyObject.h"
#include "Occlusion.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "happly.h"
#include <algorithm>
#include <cmath>
#include <memory>

PlyObject::~PlyObject() {}

PlyObject::PlyObject(char const *FileName, PlyPointCloud &p) : VA(), layout() {
  this->Load(FileName, p);
}

void PlyObject::Load(char const *FileName, PlyPointCloud &ptCloud) {
  happly::PLYData plyIn(FileName);
  // Construct the data object by reading from file
  std::vector<std::array<double, 3>> vPos = plyIn.getVertexPositions();
  std::vector<double> xNormal =
      plyIn.getElement("vertex").getProperty<double>("nx");
  std::vector<double> yNormal =
      plyIn.getElement("vertex").getProperty<double>("ny");
  std::vector<double> zNormal =
      plyIn.getElement("vertex").getProperty<double>("nz");
  auto triangleIndexes = plyIn.getFaceIndices<unsigned int>();

  // Load index data
  for (auto index : triangleIndexes) {
    for (int i = 0; i < 3; i++) {
      VertexWithResolution v;
      v.coordinate[0] = vPos[index[i]][0];
      v.coordinate[1] = vPos[index[i]][1];
      v.coordinate[2] = vPos[index[i]][2];
      v.coordinate[3] = 1.0;

      v.normal[0] = xNormal[index[i]];
      v.normal[1] = yNormal[index[i]];
      v.normal[2] = zNormal[index[i]];

      vertex_data.push_back(v);
    }
  }

  determineOcclusion(*this, ptCloud);

  // Load material data
  Material m;
  m.ambient = glm::vec3(0.02, 0.02, 0.02);
  m.diffuse = glm::vec3(1.0, 0.0, 0.0);
  m.specular = glm::vec3(1.0, 1.0, 1.0);
  m.shine = 60;
  materials.push_back(m);

  // setup VB, VA & IB
  VB = std::make_shared<VertexBuffer>(
      (GLfloat *)vertex_data.data(),
      vertex_data.size() * sizeof(VertexWithResolution), vertex_data.size());

  layout.AddFloat(4); // coordinates
  layout.AddFloat(3); // normals
  layout.AddFloat(3); // point count
  // layout.AddFloat(1); // surface area
  // layout.AddFloat(1); // res
  VA.AddBuffer(*VB, layout);
}

GLuint PlyObject::numTriangles() { return vertex_data.size() / 3; }

GLuint PlyObject::numVertices() { return vertex_data.size(); }
