#pragma once

#define GLM_FORCE_SWIZZLE
#include "PlyObject.h"
#include "PlyPointCloud.h"

void determineOcclusion(PlyObject &reference, PlyPointCloud &pointCloud);
float getOcclusionIndex(PlyObject &reference, float points_per_area_threshold);
