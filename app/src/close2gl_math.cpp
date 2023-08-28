#include "close2gl_math.h"
#include "Debug.h"
#include <cmath>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#define XYZ(x) x[0], x[1], x[2]

#define EPS 1e-12

glm::vec3 c2gl::bufferToVec3(float *buffer) {
  return glm::vec3(buffer[0], buffer[1], buffer[2]);
}

glm::vec4 c2gl::bufferToVec4(float *buffer) {
  return glm::vec4(bufferToVec3(buffer), 1.0);
}
glm::mat4 c2gl::rotateX(float theta) {
  float cos = glm::cos(theta), sin = glm::sin(theta);

  return glm::transpose(glm::mat4({
      1, 0.0, 0.0, 0.0,    // x
      0.0, cos, -sin, 0.0, // y
      0.0, sin, cos, 0.0,  // z
      0.0, 0.0, 0.0, 1.0,  // w
  }));
}

glm::mat4 c2gl::rotateY(float theta) {
  float cos = glm::cos(theta), sin = glm::sin(theta);

  return glm::transpose(glm::mat4({
      cos, 0.0, sin, 0.0,  // x
      0.0, 1.0, 0.0, 0.0,  // y
      -sin, 0.0, cos, 0.0, // z
      0.0, 0.0, 0.0, 1.0,  // w
  }));
}

glm::mat4 c2gl::rotateZ(float theta) {
  float cos = glm::cos(theta), sin = glm::sin(theta);

  return glm::transpose(glm::mat4({
      cos, -sin, 0.0, 0.0, // y
      sin, cos, 0.0, 0.0,  // z
      0.0, 0.0, 1.0, 0.0,  // w
      0.0, 0.0, 0.0, 1.0   // w
  }));
}

glm::mat4 c2gl::translate(glm::vec3 translation) {
  return glm::transpose(glm::mat4({
      1.0, 0.0, 0.0, translation.x, // y
      0.0, 1.0, 0.0, translation.y, // z
      0.0, 0.0, 1.0, translation.z, // w
      0.0, 0.0, 0.0, 1.0            // w
  }));
}

glm::mat4 c2gl::scale(float factor) {
  return glm::transpose(glm::mat4({
      factor, 0.0, 0.0, 0.0, // x
      0.0, factor, 0.0, 0.0, // y
      0.0, 0.0, factor, 0.0, // z
      0.0, 0.0, 0.0, 1.0,    // w
  }));
}

glm::mat4 c2gl::lookAt(glm::vec3 eye, glm::vec3 look_at, glm::vec3 up) {

  glm::vec3 n = glm::normalize(eye - look_at);
  glm::vec3 u = glm::cross(glm::normalize(up), n);
  glm::vec3 v = glm::cross(n, u);

  // return glm::transpose(glm::mat4({
  //     u.x, u.y, u.z, -glm::dot(eye, u), // x
  //     v.x, v.y, v.z, -glm::dot(eye, v), // y
  //     n.x, n.y, n.z, -glm::dot(eye, n), // z
  //     0.0, 0.0, 0.0, 1.0                // w
  // }));
  return glm::transpose(glm::mat4({
      u.x, v.x, n.x, -glm::dot(eye, u), // x
      u.y, v.y, n.y, -glm::dot(eye, v), // y
      u.z, v.z, n.z, -glm::dot(eye, n), // z
      0.0, 0.0, 0.0, 1.0                // w
  }));
}

glm::mat4 c2gl::perspective(float fov_y, float aspect_ratio, float near,
                            float far) {

  float tan = glm::tan(fov_y / 2);

  return glm::transpose(glm::mat4({
      1 / (aspect_ratio * tan), 0.0, 0.0, 0.0, // x
      0.0, 1 / tan, 0.0, 0.0,                  // y
      0.0, 0.0, (far + near) / (near - far),   //...
      (2 * far * near) / (near - far),         // z
      0.0, 0.0, -1, 0.0                        // w
  }));
}

glm::mat4 c2gl::viewport() {

  return glm::transpose(glm::mat4({
      (float)WINDOW_WIDTH / 2, 0.0, 0.0, (float)WINDOW_WIDTH / 2,   // x
      0.0, (float)WINDOW_HEIGHT / 2, 0.0, (float)WINDOW_HEIGHT / 2, // y
      0.0, 0.0, 1.0, 0,                                             // z
      0.0, 0.0, 0.0, 1.0                                            // w
  }));
}

std::vector<Vertex> c2gl::project_vertices(glm::mat4 modelViewProj,
                                           std::vector<Vertex> vertex_data,
                                           bool is_CW) {
  std::vector<Vertex> output;
  output.reserve(vertex_data.size() * 3);
  glm::vec4 coord4, ndc_coord, projected_coord;

  bool skip_triangle = false;
  int i = -1;

  Vertex triangle_points[3];
  for (auto v : vertex_data) {
    i += 1;

    // If skip_triangle is toggled, keep going until the next multiple of 3
    // then toggle off skip_triangle
    if (skip_triangle && (i % 3 != 0)) {
      continue;
    } else {
      skip_triangle = false;
    }

    coord4 = glm::vec4(v.coordinate[0], v.coordinate[1], v.coordinate[2], 1.0);
    ndc_coord = modelViewProj * coord4;

    // Point is behind or next to camera
    if (ndc_coord.w <= 0) {
      skip_triangle = true;
      continue;
    }

    // Perspective division
    projected_coord = ndc_coord / ndc_coord.w;

    // Point is too near or too far
    if (projected_coord.z < -1 || projected_coord.z > 1) {
      skip_triangle = true;
      continue;
    }

    v.coordinate[0] = projected_coord.x;
    v.coordinate[1] = projected_coord.y;
    v.coordinate[2] = projected_coord.z;
    triangle_points[i % 3] = v;

    if (i % 3 == 2) // last point in this triangle
    {
      float signed_area =
          triangle_points[0].coordinate[0] * triangle_points[1].coordinate[1] -
          triangle_points[1].coordinate[0] * triangle_points[0].coordinate[1] +

          triangle_points[1].coordinate[0] * triangle_points[2].coordinate[1] -
          triangle_points[2].coordinate[0] * triangle_points[1].coordinate[1] +

          triangle_points[2].coordinate[0] * triangle_points[0].coordinate[1] -
          triangle_points[0].coordinate[0] * triangle_points[2].coordinate[1];

      // if ((!is_CW && (signed_area < 0)) || (is_CW && (signed_area > 0))) {
      //   skip_triangle = true;
      //   continue;
      // } else {
      output.push_back(triangle_points[0]);
      output.push_back(triangle_points[1]);
      output.push_back(triangle_points[2]);
      // }
    }
  }

  return output;
}

void c2gl::setPixel(unsigned char *image, uint i, uint j, unsigned char r,
                    unsigned char g, unsigned char b) {

  image[(j + i * WINDOW_WIDTH) * 4 + 0] = r;
  image[(j + i * WINDOW_WIDTH) * 4 + 1] = g;
  image[(j + i * WINDOW_WIDTH) * 4 + 2] = b;
  image[(j + i * WINDOW_WIDTH) * 4 + 3] = 0xff;
}

void c2gl::point2Pixel(unsigned char *image, glm::vec4 p, unsigned char r,
                       unsigned char g, unsigned char b) {
  glm::vec4 pixel = viewport() * p;
  setPixel(image, (uint)pixel.y, (uint)pixel.x, r, g, b);
}

void point2PixelFloat(unsigned char *image, glm::vec4 p, float r, float g,
                      float b) {
  c2gl::point2Pixel(image, p, (unsigned char)(fmax(fmin(255 * r, 255), 0)),
                    (unsigned char)(fmax(fmin(255 * g, 255), 0)),
                    (unsigned char)(fmax(fmin(255 * b, 255), 0)));
}

void c2gl::_getVertices(const std::vector<Vertex> &points, uint i,
                        glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2) {
  v0.x = points[i + 0].coordinate[0];
  v0.y = points[i + 0].coordinate[1];
  v0.z = points[i + 0].coordinate[2];
  v1.x = points[i + 1].coordinate[0];
  v1.y = points[i + 1].coordinate[1];
  v1.z = points[i + 1].coordinate[2];
  v2.x = points[i + 2].coordinate[0];
  v2.y = points[i + 2].coordinate[1];
  v2.z = points[i + 2].coordinate[2];
}

void c2gl::raster_points(Vertex _v0, Vertex _v1, Vertex _v2, glm::vec3 c0,
                         glm::vec3 c1, glm::vec3 c2, unsigned char *image) {
  point2PixelFloat(image, c2gl::bufferToVec4(_v0.coordinate), c0.r, c0.g, c0.b);
  point2PixelFloat(image, c2gl::bufferToVec4(_v1.coordinate), c1.r, c1.g, c1.b);
  point2PixelFloat(image, c2gl::bufferToVec4(_v2.coordinate), c2.r, c2.g, c2.b);
}

void raster_line(glm::vec2 start, glm::vec2 end, unsigned char *image) {
  glm::vec2 accum = start;
  glm::vec2 dir = end - start;
  glm::vec2 dir_norm = glm::normalize(end - start);
  for (int i = 0; i <= glm::round(glm::length(dir)); i++) {
    c2gl::setPixel(image, (uint)glm::round(accum.y) % WINDOW_HEIGHT,
                   (uint)glm::round(accum.x) % WINDOW_WIDTH, 0xff, 0, 0);
    accum += dir_norm;
  }
}

void c2gl::raster_wireframe(Vertex _v0, Vertex _v1, Vertex _v2, glm::vec3 c0,
                            glm::vec3 c1, glm::vec3 c2, unsigned char *image) {
  glm::vec4 v0 = viewport() * c2gl::bufferToVec4(_v0.coordinate);
  glm::vec4 v1 = viewport() * c2gl::bufferToVec4(_v1.coordinate);
  glm::vec4 v2 = viewport() * c2gl::bufferToVec4(_v2.coordinate);
  raster_line(v0, v1, image);
  raster_line(v1, v2, image);
  raster_line(v2, v0, image);

  // glm::vec4 a = v1 - v0;
  // glm::vec4 b = v2 - v1;
  // glm::vec4 c = v2 - v0;
  //
  // glm::vec4 a_norm = a / glm::length(a);
  // glm::vec4 b_norm = b / glm::length(b);
  // glm::vec4 c_norm = c / glm::length(c);
  //
  // glm::vec4 accum(0);
  //
  // accum = v0;
  // for (int i = 0; i < (int)glm::length(a); i++) {
  //   setPixel(image, (uint)accum.y, (uint)accum.x, 0xff, 0, 0);
  //   accum += a_norm;
  // }
  // accum = v0;
  // for (int i = 0; i < (int)glm::length(c); i++) {
  //   setPixel(image, (uint)accum.y, (uint)accum.x, 0xff, 0, 0);
  //   accum += c_norm;
  // }
  // accum = v1;
  // for (int i = 0; i < (int)glm::length(b); i++) {
  //   setPixel(image, (uint)accum.y, (uint)accum.x, 0xff, 0, 0);
  //   accum += b_norm;
  // }
}

float get_lambda(glm::vec4 v, glm::vec4 dir, float ycurr) {
  if (glm::abs(v.y) < EPS) {
    return -1; // lambda < 0 -> 'out of triangle'
  }
  float lambda = (ycurr - v.y) / dir.y;
  if (lambda > 1.1) {
    return -1; // lambda < 0 -> 'out of triangle' (to simplify)
  }
  return lambda; // lambda < 0 -> 'out of triangle'
}

void c2gl::raster_solid(Vertex _v0, Vertex _v1, Vertex _v2, glm::vec3 c0,
                        glm::vec3 c1, glm::vec3 c2, unsigned char *image) {

  glm::vec4 v0 = viewport() * c2gl::bufferToVec4(_v0.coordinate);
  glm::vec4 v1 = viewport() * c2gl::bufferToVec4(_v1.coordinate);
  glm::vec4 v2 = viewport() * c2gl::bufferToVec4(_v2.coordinate);

  glm::vec4 a = v1 - v0;
  glm::vec4 b = v2 - v1;
  glm::vec4 c = v0 - v2;

  float start_y = glm::min(v0.y, glm::min(v1.y, v2.y));
  float end_y = glm::max(v0.y, glm::max(v1.y, v2.y));

  float lambda_a, lambda_b, lambda_c;
  char k = 0;
  float active_lambdas[3] = {0, 0, 0};
  float active_x[3] = {0, 0, 0};
  float start_x = 0, end_x = 0;

  if (glm::abs(a.x) < 1e-4) {
    raster_line(v0, v1, image);
  } else if (glm::abs(b.x) < 1e-4) {
    raster_line(v2, v1, image);
  } else if (glm::abs(c.x) < 1e-4) {
    raster_line(v0, v2, image);
  }
  for (uint i = (uint)glm::round(start_y); i <= (uint)glm::round(end_y); i++) {
    lambda_a = get_lambda(v0, a, i);
    lambda_b = get_lambda(v1, b, i);
    lambda_c = get_lambda(v2, c, i);
    k = 0;
    if (lambda_a > 0) {
      active_lambdas[k] = lambda_a;
      active_x[k] = lambda_a * a.x + v0.x;
      k++;
    }
    if (lambda_b > 0) {
      active_lambdas[k] = lambda_b;
      active_x[k] = lambda_b * b.x + v1.x;
      k++;
    }
    if (lambda_c > 0) {
      active_lambdas[k] = lambda_c;
      active_x[k] = lambda_c * c.x + v2.x;
      k++;
    }
    // only two active edges
    if (k < 2)
      continue;
    if (k > 2) {
      start_x = glm::min(active_x[0], glm::min(active_x[1], active_x[2]));
      end_x = glm::max(active_x[0], glm::max(active_x[1], active_x[2]));
    } else {
      start_x = glm::min(active_x[0], active_x[1]);
      end_x = glm::max(active_x[0], active_x[1]);
    }
    raster_line(glm::vec2(start_x, i), glm::vec2(end_x, i), image);
  }
}

glm::vec3 getVertexColorPhong(Vertex p, ModelViewProj MVP, Light light,
                              glm::vec3 ambient, glm::vec3 diffuse,
                              glm::vec3 specular, float shine) {

  glm::vec4 vPos(XYZ(p.coordinate), 1.0);
  glm::vec3 vNormal(XYZ(p.normal));

  glm::vec4 v_pos4 = MVP.V * MVP.M * vPos;
  glm::vec3 v_pos = glm::vec3(v_pos4);
  glm::vec3 normal = glm::normalize(
      glm::transpose(glm::inverse(glm::mat3(MVP.V * MVP.M))) * vNormal);

  glm::vec3 light_pos = glm::vec3(MVP.V * glm::vec4(light.position, 1.0));
  glm::vec3 v_to_light = glm::normalize(light_pos - v_pos);

  glm::vec3 v_to_eye = glm::normalize(-v_pos);
  glm::vec3 v_reflection = glm::normalize(-glm::reflect(v_to_light, normal));

  glm::vec3 ambient_light = ambient;
  glm::vec3 diffuse_light = glm::max(glm::dot(normal, v_to_light), (GLfloat)0) *
                            diffuse * light.color * light.strength;
  glm::vec3 specular_light =
      glm::pow(glm::max(glm::dot(v_reflection, v_to_eye), (GLfloat)0.0),
               shine) *
      specular * light.color * light.strength;

  return ambient_light + diffuse_light + specular_light;
}

void c2gl::raster(std::vector<Vertex> points, unsigned char *image,
                  const Controls *controls, Light light, ModelViewProj MVP,
                  std::vector<Material> materials) {
  glm::vec3 v0, v1, v2;
  glm::vec3 c0, c1, c2;
  memset(image, 0, WINDOW_WIDTH * WINDOW_HEIGHT * 4);
  for (int i = 0; i < points.size(); i += 3) {
    c2gl::_getVertices(points, i, v0, v1, v2);
    if (std::abs(v0.x) > 1 || std::abs(v0.y) > 1 || std::abs(v1.x) > 1 ||
        std::abs(v1.y) > 1 || std::abs(v2.x) > 1 || std::abs(v2.y) > 1) {
      continue;
    }

    c0 = getVertexColorPhong(points[i + 0], MVP, light,
                             materials[points[i + 0].material_index].ambient,
                             materials[points[i + 0].material_index].diffuse,
                             materials[points[i + 0].material_index].specular,
                             materials[points[i + 0].material_index].shine);
    c1 = getVertexColorPhong(points[i + 1], MVP, light,
                             materials[points[i + 1].material_index].ambient,
                             materials[points[i + 1].material_index].diffuse,
                             materials[points[i + 1].material_index].specular,
                             materials[points[i + 1].material_index].shine);
    c2 = getVertexColorPhong(points[i + 2], MVP, light,
                             materials[points[i + 2].material_index].ambient,
                             materials[points[i + 2].material_index].diffuse,
                             materials[points[i + 2].material_index].specular,
                             materials[points[i + 2].material_index].shine);

    std::cout << glm::to_string(c0) << std::endl;

    switch (controls->chosen_render) {
    case POINTS:
      raster_points(points[i], points[i + 1], points[i + 2], c0, c1, c2, image);
      break;
    case WIREFRAME:
      raster_wireframe(points[i], points[i + 1], points[i + 2], c0, c1, c2,
                       image);
      break;
    default:
    case SOLID:
      raster_solid(points[i], points[i + 1], points[i + 2], c0, c1, c2, image);
      break;
    }
  }
}
