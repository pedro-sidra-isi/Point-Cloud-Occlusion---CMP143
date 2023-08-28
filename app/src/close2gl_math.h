#pragma once
#include "util.h"
#include <cmath>
#include <glm/gtx/string_cast.hpp>
#include <vector>

typedef struct _mvp {
  glm::mat4 M;
  glm::mat4 V;
  glm::mat4 P;
  glm::mat4 MVP;
} ModelViewProj;

namespace c2gl {

glm::mat4 rotateX(float theta);

glm::mat4 rotateY(float theta);

glm::mat4 rotateZ(float theta);

glm::mat4 translate(glm::vec3 translation);

glm::mat4 scale(float factor);

glm::mat4 lookAt(glm::vec3 eye, glm::vec3 look_at, glm::vec3 up);

glm::mat4 perspective(float fov_y, float aspect_ratio, float near, float far);

glm::mat4 viewport();

std::vector<Vertex> project_vertices(glm::mat4 modelViewProj,
                                     std::vector<Vertex> vertex_data,
                                     bool is_CW);

void setPixel(unsigned char *image, uint i, uint j, unsigned char r,
              unsigned char g, unsigned char b);

void point2Pixel(unsigned char *image, glm::vec4 p, unsigned char r,
                 unsigned char g, unsigned char b);

void _getVertices(const std::vector<Vertex> &points, uint i, glm::vec3 &v0,
                  glm::vec3 &v1, glm::vec3 &v2);

void raster(std::vector<Vertex> points, unsigned char *image,
            const Controls *controls, Light ligth, ModelViewProj MVP,
            std::vector<Material> materials);

void raster_points(Vertex v0, Vertex v1, Vertex v2, glm::vec3 c0, glm::vec3 c1,
                   glm::vec3 c2, unsigned char *image);
void raster_wireframe(Vertex v0, Vertex v1, Vertex v2, glm::vec3 c0,
                      glm::vec3 c1, glm::vec3 c2, unsigned char *image);
void raster_solid(Vertex v0, Vertex v1, Vertex v2, glm::vec3 c0, glm::vec3 c1,
                  glm::vec3 c2, unsigned char *image);

glm::vec3 bufferToVec3(float *buffer);
glm::vec4 bufferToVec4(float *buffer);
} // namespace c2gl
