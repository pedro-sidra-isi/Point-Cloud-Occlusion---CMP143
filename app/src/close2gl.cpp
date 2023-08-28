#include "close2gl_math.h"
#include "glm/gtx/string_cast.hpp"
#include "opengl_reference.h"
#include "stb_image.h"
#include <chrono>
#include <cmath>
#include <ctime>

void c2gl_updateFPS(Controls *controls) {
  static std::chrono::time_point<std::chrono::high_resolution_clock> oldTime;
  static int count = 0;
  count++;

  if (count < 100) {
    return;
  }

  int dTime = std::chrono::duration_cast<std::chrono::microseconds>(
                  std::chrono::high_resolution_clock::now() - oldTime)
                  .count();
  controls->frame_time_close2gl = (float)dTime / 1000000;
  oldTime = std::chrono::high_resolution_clock::now();
}
// ======= Main Function

ModelViewProj init_modelViewProj(Controls *controls, glm::vec3 object_center) {
  glm::mat4 model, view, proj;

  float s = 0.5;

  // Model is moved to origin, THEN scaled down
  model = c2gl::scale(s) * c2gl::translate(-object_center);

  glm::vec3 eye(0, 0, -700), center(0, 0, 0), up(0, 1, 0);
  if (controls->look_at_object) {
    // Looking at object: eye moves around object, points to origin
    eye = c2gl::rotateX(controls->camera_rotation.x) *
          c2gl::rotateY(controls->camera_rotation.y) *
          c2gl::rotateZ(controls->camera_rotation.z) *
          c2gl::translate(controls->camera_translation) *
          glm::vec4(eye.x, eye.y, eye.z, 1);
    up = c2gl::rotateX(controls->camera_rotation.x) *
         c2gl::rotateY(controls->camera_rotation.y) *
         c2gl::rotateZ(controls->camera_rotation.z) * glm::vec4(0, 1, 0, 1);
    view = c2gl::lookAt(eye, center, up);
  } else {
    // Not looking at object: camera system moves around its axis
    view = c2gl::lookAt(eye, center, up);
    view = c2gl::rotateX(controls->camera_rotation.x) *
           c2gl::rotateY(controls->camera_rotation.y) *
           c2gl::rotateZ(controls->camera_rotation.z) *
           c2gl::translate(controls->camera_translation) * view;
  }

  proj = c2gl::perspective(controls->fovy, controls->aspect_ratio,
                           controls->near_clip, controls->far_clip);
  ModelViewProj out;
  out.M = model;
  out.V = view;
  out.P = proj;
  out.MVP = proj * view * model;
  return out;
}

void close2gl(Controls *controls) {
  // Init separate window for close2GL
  GLFWwindow *window = init_GL("Close2GL");

  // Load our giselle
  Object giselle("../bin/giselle.in");
  // Close2GL Giselle is blue
  giselle.materials[0].diffuse = glm::vec3(0, 0, 1.0);
  // For resetting material later
  Material backup_material = giselle.materials[0];

  // Center of object and object bounds
  BoundingBox bbox =
      calculate_bbox(giselle.vertex_data.data(), giselle.numVertices());
  glm::vec3 center_of_object = (bbox.min + bbox.max) / 2.0f;

  // Only one light in this scene
  Light light_0;

  // Construct shaders and point to the default one
  Shader basicShader("../res/shaders/Basic.shader");
  Shader *use_shader = &basicShader;
  Shader textureShader("../res/shaders/Texture.shader");

  // Renderer takes care of binding, clearing buffers and drawing arrays
  Renderer renderer;

  clock_t start_ticks, delta_ticks;

  FullImageBuffer fib;

  unsigned char *image = new unsigned char[WINDOW_WIDTH * WINDOW_HEIGHT * 4];
  memset(image, 0, WINDOW_WIDTH * WINDOW_HEIGHT * 4);

  while (!glfwWindowShouldClose(window)) {
    start_ticks = clock();

    // Clear frame
    renderer.Clear();

    // ====== Update Lighting & Materials  ======
    giselle.materials[0].diffuse = controls->object_colors;
    light_0 = {
        controls->light_translation, // position
        controls->light_color,       // color
        1.0f,                        // strength
    };

    switch (controls->chosen_shader) {
    default:
    case GOURAUD_AD:
      // TODO
      break;
    case GOURAUD_ADS:
      // TODO
      break;
    case PHONG:
      // TODO
      break;
    }

    ModelViewProj MVP = init_modelViewProj(controls, center_of_object);

    std::vector<Vertex> projected_points = c2gl::project_vertices(
        MVP.MVP, giselle.vertex_data, controls->orientation_cw);

    // ====== Render and swap buffers  ======
    if (controls->close2gl_raster) {
      use_shader = &textureShader;
      use_shader->Bind();
      c2gl::raster(projected_points, image, controls, light_0, MVP,
                   giselle.materials);
      renderer.DrawFullscreenImage(fib, textureShader, image);
    } else {
      VertexArray VA;
      VertexBuffer VB(projected_points.data(),
                      projected_points.size() * sizeof(Vertex),
                      projected_points.size());
      VertexBufferLayout layout;
      layout.AddFloat(4);       // coordinates
      layout.AddFloat(3);       // normals
      layout.AddUnsignedInt(1); // materials
      VA.AddBuffer(VB, layout);
      use_shader = &basicShader;
      use_shader->Bind();
      use_shader->SetUniform3f("color",
                               glm::value_ptr(giselle.materials[0].diffuse));
      renderer.Draw(VA, *use_shader, VB.getCount(), controls->chosen_render);
    }

    GLCall(glfwSwapBuffers(window));
    GLCall(glfwPollEvents());
    c2gl_updateFPS(controls);
  }

  glfwDestroyWindow(window);
  glfwTerminate();
}
