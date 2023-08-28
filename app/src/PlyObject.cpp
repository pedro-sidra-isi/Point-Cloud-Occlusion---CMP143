#include "PlyObject.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include <algorithm>
#include <cmath>
#include <happly.h>
#include <memory>

PlyObject::~PlyObject() {}

PlyObject::PlyObject(char const *FileName) : VA(), layout() {
  this->Load(FileName);
}

void PlyObject::Load(char const *FileName) {
  // Construct the data object by reading from file
  happly::PLYData plyIn("my_mesh_file.ply");

  std::vector<std::array<double, 3>> vPos = plyIn.getVertexPositions();
  std::vector<std::vector<size_t>> fInd = plyIn.getFaceIndices<size_t>();
}

GLuint PlyObject::numTriangles() { return NumTris; }

GLuint PlyObject::numVertices() { return 3 * NumTris; }
