#pragma once
#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#define MAX_MATERIAL_COUNT 255
#define FLOATS_PER_TRI 9
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale,
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/vec3.hpp>   // glm::vec3
#include <glm/vec4.hpp>   // glm::vec4
#include <glm/vector_relational.hpp>
#include <iostream>
#include <memory>

#define MAX_FLOAT std::numeric_limits<float>::max()
#define MIN_FLOAT std::numeric_limits<float>::min()

#define BUFFER_OFFSET(a) ((void *)(a))

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 600
#define ASPECT_RATIO float(WINDOW_WIDTH) / float(WINDOW_HEIGHT)

enum LightingType {
  GOURAUD_AD = 0,
  GOURAUD_ADS,
  PHONG,
  RESOLUTION,
  RESOLUTION_THRESHOLD,
  NUM_SHADERS
};
enum RenderType {
  POINTS = GL_POINTS,
  WIREFRAME = GL_LINES,
  SOLID = GL_TRIANGLES,
  NUM_RENDER_TYPES
};

// UI - controled parameters
typedef struct controls_ {

  float resolution_threshold = 0;
  float occlusion_index = 0;
  glm::vec3 camera_translation = {0, 0, 0};
  glm::vec3 camera_rotation = {0, 0, 0};
  glm::vec3 object_gradual_rotation = {0, 0, 0};
  glm::vec3 object_colors = {1, 0, 0};

  glm::vec3 light_color = {1, 1, 1};
  glm::vec3 light_translation = {2.0, 2.0, 200.0};

  GLfloat fovy = glm::radians(45.0f);
  bool look_at_object = false;
  bool spin_object = false;
  int orientation_cw = 0;
  GLfloat near_clip = 0.1f;
  GLfloat far_clip = 2000.0f;
  GLfloat aspect_ratio = ASPECT_RATIO;
  LightingType chosen_shader = PHONG;
  RenderType chosen_render = SOLID;

  bool close2gl_raster = true;

  float frame_time_opengl;
  float frame_time_close2gl;
} Controls;

typedef struct {
  GLenum type;
  const char *filename;
  GLuint shader;
} ShaderInfo;

typedef struct {
  glm::vec3 v0, v1, v2;
  GLuint material_index_0, material_index_1, material_index_2;
  glm::vec3 normal_0, normal_1, normal_2;
  glm::vec3 face_normal;
} Triangle;

typedef struct {
  glm::vec3 ambient, diffuse, specular;
  GLfloat shine;
} Material;

typedef struct {
  float coordinate[4];
  float normal[3];
  uint material_index;
} Vertex;

typedef struct {
  float coordinate[4];
  float normal[3];
  float properties[3];
  // point_count;
  // float surface_area;
  // float resolution;
} VertexWithResolution;

typedef struct {
  uint i;
  uint j;
  uint k;
} VertexIndex;

typedef struct {
  glm::vec3 position;
  glm::vec3 color;
  GLfloat strength;
} Light;

typedef struct bbox {
  glm::vec3 min = {MAX_FLOAT, MAX_FLOAT, MAX_FLOAT};
  glm::vec3 max = {MIN_FLOAT, MIN_FLOAT, MIN_FLOAT};
} BoundingBox;

static const GLchar *ReadShader(const char *filename);
GLuint LoadShaders(ShaderInfo *shaders);

BoundingBox calculate_bbox(Vertex *vertices, GLuint NumVertices);
BoundingBox calculate_bbox(VertexWithResolution *vertices, GLuint NumVertices);
