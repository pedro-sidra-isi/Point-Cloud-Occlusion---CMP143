#include "Occlusion.h"
#include <glm/gtx/string_cast.hpp>

#define GLM_FORCE_SWIZZLE
#include "PlyObject.h"
#include "PlyPointCloud.h"
#define XYZ(x) x[0], x[1], x[2]

void determineOcclusion(PlyObject &reference, PlyPointCloud &pointCloud) {
  glm::vec3 v0, v1, v2, u, v, face_normal;
  glm::vec4 p, projected;
  p.w = 1.0f;
  glm::mat4 trics_to_worldcs(1.0f), transform(1.0f);
  float points_inside_triangle = 0;
  float surface_area;
  float normal_eps_pos = 0.01, normal_eps_neg = 0.01;
  reference.max_vertex_values.properties[1] = 0;
  reference.max_vertex_values.properties[0] = 0;
  reference.max_vertex_values.properties[2] = 0;
  for (uint i = 0; i < reference.vertex_data.size(); i += 3) {
    v0.x = reference.vertex_data[i + 0].coordinate[0];
    v1.x = reference.vertex_data[i + 1].coordinate[0];
    v2.x = reference.vertex_data[i + 2].coordinate[0];
    v0.y = reference.vertex_data[i + 0].coordinate[1];
    v1.y = reference.vertex_data[i + 1].coordinate[1];
    v2.y = reference.vertex_data[i + 2].coordinate[1];
    v0.z = reference.vertex_data[i + 0].coordinate[2];
    v1.z = reference.vertex_data[i + 1].coordinate[2];
    v2.z = reference.vertex_data[i + 2].coordinate[2];

    u = v1 - v0;
    v = v2 - v1;
    face_normal = glm::cross(u, v);
    surface_area = glm::length(face_normal) / 2;
    face_normal = -face_normal / (2 * surface_area);

    trics_to_worldcs[0] = glm::vec4(v1 - v0, 0.0);
    trics_to_worldcs[1] = glm::vec4(v2 - v0, 0.0);
    trics_to_worldcs[2] = glm::vec4(face_normal, 0.0);
    trics_to_worldcs[3] = glm::vec4(v0, 1.0);

    transform = glm::inverse(trics_to_worldcs);

    points_inside_triangle = 0;
    for (int j = 0; j < pointCloud.point_data.size(); j++) {
      p.x = pointCloud.point_data[j].coordinate[0];
      p.y = pointCloud.point_data[j].coordinate[1];
      p.z = pointCloud.point_data[j].coordinate[2];

      projected = transform * p;
      if ((projected.x >= 0) && (projected.y >= 0) && // positive axes
          (projected.y + projected.x <= 1) &&         // below diagonal
          (((projected.z > 0) &&
            (projected.z <= normal_eps_pos)) // maximum 'outside' distance
           || ((projected.z < 0) &&
               (projected.z >= normal_eps_neg)))) { // maximum "inside" distance
        // is inside triangle
        points_inside_triangle += 1;
      }
    }
    reference.vertex_data[i + 0].properties[0] = points_inside_triangle;
    reference.vertex_data[i + 0].properties[1] = surface_area;
    reference.vertex_data[i + 0].properties[2] =
        (float)points_inside_triangle / surface_area;
    // std::cout << (float)points_inside_triangle / surface_area << std::endl;

    reference.max_vertex_values.properties[0] = glm::max(
        points_inside_triangle, reference.max_vertex_values.properties[0]);
    reference.max_vertex_values.properties[1] =
        glm::max(surface_area, reference.max_vertex_values.properties[1]);
    reference.max_vertex_values.properties[2] =
        glm::max((float)points_inside_triangle / surface_area,
                 reference.max_vertex_values.properties[2]);

    // dumb
    reference.vertex_data[i + 1].properties[0] =
        reference.vertex_data[i + 0].properties[0];
    reference.vertex_data[i + 1].properties[1] =
        reference.vertex_data[i + 0].properties[1];
    reference.vertex_data[i + 1].properties[2] =
        reference.vertex_data[i + 0].properties[2];
    reference.vertex_data[i + 2].properties[0] =
        reference.vertex_data[i + 1].properties[0];
    reference.vertex_data[i + 2].properties[1] =
        reference.vertex_data[i + 1].properties[1];
    reference.vertex_data[i + 2].properties[2] =
        reference.vertex_data[i + 1].properties[2];
  }
}

float getOcclusionIndex(PlyObject &reference, float points_per_area_threshold) {

  glm::vec3 v0, v1, v2, u, v, face_normal;
  glm::vec4 p, projected;
  p.w = 1.0f;
  glm::mat4 trics_to_worldcs(1.0f), transform(1.0f);
  float normal_eps_pos = 0.01, normal_eps_neg = 0.01;

  float points_inside_triangle, surface_area, resolution;
  float total_surface_area = 0, covered_area = 0;

  for (uint i = 0; i < reference.vertex_data.size(); i += 3) {
    points_inside_triangle = reference.vertex_data[i].properties[0];
    surface_area = reference.vertex_data[i].properties[1];
    resolution = reference.vertex_data[i].properties[2];

    if (points_inside_triangle > points_per_area_threshold) {
      covered_area += surface_area;
    }
    total_surface_area += surface_area;
  }

  return 1 - (covered_area / total_surface_area);
}
