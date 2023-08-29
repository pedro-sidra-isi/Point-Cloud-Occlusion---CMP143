#include "PlyObject.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "happly.h"
#include <algorithm>
#include <cmath>
#include <memory>

PlyObject::~PlyObject() {}

PlyObject::PlyObject(char const *FileName) : VA(), layout() {
  this->Load(FileName);
}

void PlyObject::Load(char const *FileName) {
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

  // Load vertex data
  for (int i = 0; i < vPos.size(); i++) {
    Vertex v;

    v.coordinate[0] = vPos[i][0];
    v.coordinate[1] = vPos[i][1];
    v.coordinate[2] = vPos[i][2];
    v.coordinate[3] = 1.0;

    v.normal[0] = xNormal[i];
    v.normal[1] = yNormal[i];
    v.normal[2] = zNormal[i];

    v.material_index = 0;
    vertex_data.push_back(v);
  }

  // Load index data
  for (auto index : triangleIndexes) {
    VertexIndex v;

    v.i = index[0];
    v.j = index[1];
    v.k = index[2];
    index_data.push_back(v);
  }

  // Load material data
  Material m;
  m.ambient = glm::vec3(0.02, 0.02, 0.02);
  m.diffuse = glm::vec3(1.0, 0.0, 0.0);
  m.specular = glm::vec3(1.0, 1.0, 1.0);
  m.shine = 60;
  materials.push_back(m);

  // setup VB, VA & IB
  VB = std::make_shared<VertexBuffer>((GLfloat *)vertex_data.data(),
                                      vertex_data.size() * sizeof(Vertex),
                                      vertex_data.size());
  IB = std::make_shared<IndexBuffer>((unsigned int *)index_data.data(),
                                     3 * index_data.size());

  layout.AddFloat(4);       // coordinates
  layout.AddFloat(3);       // normals
  layout.AddUnsignedInt(1); // Material index
  VA.AddBuffer(*VB, layout);

  std::cout << "COW HAS" << numTriangles() << std::endl;
}

GLuint PlyObject::numTriangles() { return index_data.size(); }

GLuint PlyObject::numVertices() { return vertex_data.size(); }
