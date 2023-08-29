#include "PlyPointCloud.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "happly.h"
#include <algorithm>
#include <cmath>
#include <memory>

PlyPointCloud::~PlyPointCloud() {}

PlyPointCloud::PlyPointCloud(char const *FileName) : VA(), layout() {
  this->Load(FileName);
}

void PlyPointCloud::Load(char const *FileName) {
  happly::PLYData plyIn(FileName);
  // Construct the data object by reading from file
  std::vector<std::array<double, 3>> vPos = plyIn.getVertexPositions();

  // Load vertex data
  for (int i = 0; i < vPos.size(); i++) {
    PCloudPoint p;

    p.coordinate[0] = vPos[i][0];
    p.coordinate[1] = vPos[i][1];
    p.coordinate[2] = vPos[i][2];
    p.coordinate[3] = 1.0;

    point_data.push_back(p);
  }

  // setup VB, VA & IB
  VB = std::make_shared<VertexBuffer>((GLfloat *)point_data.data(),
                                      point_data.size() * sizeof(PCloudPoint),
                                      point_data.size());

  layout.AddFloat(4); // coordinates
  VA.AddBuffer(*VB, layout);
}

GLuint PlyPointCloud::numPoints() { return point_data.size(); }
