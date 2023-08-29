#include "opengl_reference.h"
#include "Occlusion.h"
#include "glm/gtx/string_cast.hpp"
#include <chrono>
#include <unistd.h>

// ======= Main Function
void opengl_updateFPS(Controls *controls) {
  static std::chrono::time_point<std::chrono::high_resolution_clock> oldTime;
  static int count = 0;
  count++;

  if (count < 100) {
    return;
  }

  int dTime = std::chrono::duration_cast<std::chrono::microseconds>(
                  std::chrono::high_resolution_clock::now() - oldTime)
                  .count();
  controls->frame_time_opengl = (float)dTime / 1000000;
  oldTime = std::chrono::high_resolution_clock::now();
}

void opengl(Controls *controls) {
  // Init separate window for close2GL
  GLFWwindow *window = init_GL("OpenGL");

  // Load our giselle
  // Object giselle("../bin/giselle.in");
  PlyPointCloud neoGisPointCloud("../bin/cow_pcloud.ply");
  PlyObject neoGiselle("../bin/cow.ply", neoGisPointCloud);

  determineOcclusion(neoGiselle, neoGisPointCloud);
  // OpenGL Giselle is red
  // giselle.materials[0].diffuse = glm::vec3(1.0, 0, 0);
  // For resetting material later
  // Material backup_material = giselle.materials[0];

  // Center of object and object bounds
  BoundingBox bbox =
      calculate_bbox(neoGiselle.vertex_data.data(), neoGiselle.numVertices());
  glm::vec3 center_of_object = (bbox.min + bbox.max) / 2.0f;

  // Only one light in this scene
  Light light_0;

  // Default ModelViewProj (object is centered at origin)
  glm::mat4 Projection =
      glm::perspective(controls->fovy, controls->aspect_ratio,
                       controls->near_clip, controls->far_clip);

  // Default Eye a little bit far from origin
  glm::vec3 eye(0, -100, -700), center(0, 0, 0), up(0, 1, 0);
  glm::mat4 View = glm::lookAt(eye, center, up);
  // For later
  glm::mat4 TranslateCam, RotateX, RotateY, RotateZ;

  // Model is scaled down and moved to origin
  glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(300.0f));
  Model = glm::translate(Model, -center_of_object);
  auto rotateObj = glm::rotate(glm::mat4(1.0f), glm::pi<GLfloat>() / 2,
                               glm::vec3(0.0f, 1.0f, 0.0f));
  auto rotateObj2 = glm::rotate(glm::mat4(1.0f), glm::pi<GLfloat>() / 2,
                                glm::vec3(0.0f, 0.0f, 1.0f));
  Model = rotateObj2 * rotateObj * Model;

  glm::mat4 MVP = Projection * View * Model;

  // Construct shaders and point to the default one
  Shader shaderGouraud("../res/shaders/Gouraud.shader");
  Shader shaderPhong("../res/shaders/Phong.shader");
  Shader shaderResolution("../res/shaders/Resolution.shader");
  Shader shaderResolutionThresh("../res/shaders/ResolutionThreshold.shader");
  Shader shaderPointCloud("../res/shaders/PointCloud.shader");
  Shader *use_shader = &shaderGouraud;

  // Renderer takes care of binding, clearing buffers and drawing arrays
  Renderer renderer;

  // Setup IMGUI only on the main OpenGL thread (not Close2GL)
  setupGUI(window);
  while (!glfwWindowShouldClose(window)) {

    // Clear frame
    renderer.Clear();
    clearGUI();

    // Update ImGui only on main OpenGL thread (not Close2GL)
    updateGUI(*controls);

    controls->occlusion_index =
        getOcclusionIndex(neoGiselle, controls->resolution_threshold);

    // ====== Update ModelViewProj  ======
    // Rebuild projection with user params
    Projection = glm::perspective(controls->fovy, controls->aspect_ratio,
                                  controls->near_clip, controls->far_clip);

    // Camera translations
    TranslateCam =
        glm::translate(glm::mat4(1.0f), controls->camera_translation);
    RotateX = glm::rotate(glm::mat4(1.0f), controls->camera_rotation.x,
                          glm::vec3(1.0f, 0.0f, 0.0f));
    RotateY = glm::rotate(glm::mat4(1.0f), controls->camera_rotation.y,
                          glm::vec3(0.0f, 1.0f, 0.0f));
    RotateZ = glm::rotate(glm::mat4(1.0f), controls->camera_rotation.z,
                          glm::vec3(0.0f, 0.0f, 1.0f));

    eye = {0, 0, 700};
    center = {0, 0, 0};
    up = {0, 1, 0};

    if (controls->look_at_object) {
      // Looking at object: eye moves around object, points to origin
      eye = RotateX * RotateY * RotateZ * TranslateCam *
            glm::vec4(eye.x, eye.y, eye.z, 1);
      up = RotateX * RotateY * RotateZ * glm::vec4(0, 1, 0, 1);
      View = glm::lookAt(eye, center, up);
    } else {
      // Not looking at object: camera system moves around its axis
      View = glm::lookAt(eye, center, up);
      View = RotateX * RotateY * RotateZ * TranslateCam * View;
    }

    MVP = Projection * View * Model;

    // ====== Update Lighting & Materials  ======
    // giselle.materials[0].diffuse = controls->object_colors;

    light_0 = {
        controls->light_translation, // position
        controls->light_color,       // color
        1.0f,                        // strength
    };

    switch (controls->chosen_shader) {
    default:
    case GOURAUD_AD:
      use_shader = &shaderGouraud;
      // giselle.materials[0].specular = {0.0, 0.0, 0.0};
      break;
    case GOURAUD_ADS:
      use_shader = &shaderGouraud;
      // giselle.materials[0].specular = backup_material.specular;
      break;
    case PHONG:
      use_shader = &shaderPhong;
      break;
    case RESOLUTION:
      use_shader = &shaderResolution;
      use_shader->Bind();
      use_shader->SetUniform3f(
          "max_value", glm::value_ptr(glm::vec3(
                           (float)neoGiselle.max_vertex_values.properties[0],
                           (float)neoGiselle.max_vertex_values.properties[1],
                           (float)neoGiselle.max_vertex_values.properties[2])));
      break;
    case RESOLUTION_THRESHOLD:
      use_shader = &shaderResolutionThresh;
      use_shader->Bind();
      use_shader->SetUniform3f(
          "points_per_area_threshold",
          glm::value_ptr(
              glm::vec3((float)controls->resolution_threshold, 0.0, 0.0)));
      break;
    }

    // ====== Set Uniforms  ======
    // for (auto v : neoGiselle.vertex_data) {
    //   std::cout << v.point_count << ",";
    // }
    // std::cout << std::endl;
    setUniforms_PhongGouraud(use_shader, Model, View, Projection, light_0);
    renderer.Draw(neoGiselle, *use_shader, controls->chosen_render);

    // ====== Render and swap buffers  ======
    // renderer.Draw(giselle, *use_shader, controls->chosen_render);

    shaderPointCloud.Bind();
    shaderPointCloud.SetUniform3f("color",
                                  glm::value_ptr(glm::vec3(0.0, 0.3, 0.3)));
    setUniforms_PhongGouraud(&shaderPointCloud, Model, View, Projection,
                             light_0);
    renderer.Draw(neoGisPointCloud, shaderPointCloud);

    drawGUI();
    GLCall(glfwSwapBuffers(window));
    GLCall(glfwPollEvents());
    opengl_updateFPS(controls);
  }

  ImGui_ImplGlfw_Shutdown();
  ImGui_ImplOpenGL3_Shutdown();
  glfwDestroyWindow(window);
  glfwTerminate();
}

// GL Utility
GLFWwindow *init_GL(std::string window_name) {
  glfwInit();

  GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                        window_name.c_str(), NULL, NULL);

  glfwMakeContextCurrent(window);
  gl3wInit();

  GLCall(glEnable(GL_BLEND));
  GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLCall(glFrontFace(GL_CW));

  return window;
}

// ======= Shader loading
void setUniforms_PhongGouraud(Shader *use_shader, Object &giselle,
                              glm::mat4 Model, glm::mat4 View,
                              glm::mat4 Projection, Light light) {
  use_shader->SetUniformMat4f("model", Model);
  use_shader->SetUniformMat4f("view", View);
  use_shader->SetUniformMat4f("projection", Projection);

  use_shader->SetUniform3f("light.position", glm::value_ptr(light.position));
  use_shader->SetUniform3f("light.color", glm::value_ptr(light.color));
  use_shader->SetUniform1f("light.strength", light.strength);

  for (int i = 0; i < giselle.materials.size(); i++) {
    std::string material_string = "materials[" + std::to_string(i) + "]";
    use_shader->SetUniform3f(material_string + ".ambient",
                             glm::value_ptr(giselle.materials[i].ambient));
    use_shader->SetUniform3f(material_string + ".diffuse",
                             glm::value_ptr(giselle.materials[i].diffuse));
    use_shader->SetUniform3f(material_string + ".specular",
                             glm::value_ptr(giselle.materials[i].specular));
    use_shader->SetUniform1f(material_string + ".shine",
                             giselle.materials[i].shine);
  }
}

void setUniforms_PhongGouraud(Shader *use_shader, glm::mat4 Model,
                              glm::mat4 View, glm::mat4 Projection,
                              Light light) {
  use_shader->Bind();
  use_shader->SetUniformMat4f("model", Model);
  use_shader->SetUniformMat4f("view", View);
  use_shader->SetUniformMat4f("projection", Projection);

  use_shader->SetUniform3f("light.position", glm::value_ptr(light.position));
  use_shader->SetUniform3f("light.color", glm::value_ptr(light.color));
  use_shader->SetUniform1f("light.strength", light.strength);

  std::string material_string = "materials[0]";
  use_shader->SetUniform3f(material_string + ".ambient",
                           glm::value_ptr(glm::vec3(0.02, 0.02, 0.02)));
  use_shader->SetUniform3f(material_string + ".diffuse",
                           glm::value_ptr(glm::vec3(1., 0.0, .0)));
  use_shader->SetUniform3f(material_string + ".specular",
                           glm::value_ptr(glm::vec3(1., 1.0, 1.0)));
  use_shader->SetUniform1f(material_string + ".shine", 60);
}

// ======= GUI Definitions
//
void updateGUI(Controls &controls) {

  ImGui::Text("Point Occlusion");
  ImGui::SliderFloat("Points per Triangle Threshold",
                     &controls.resolution_threshold, 0, 50000);
  ImGui::Text("Occlusion index = %f", controls.occlusion_index);

  ImGui::Text("Camera Controls");
  // object
  ImGui::SliderFloat3("Translation",
                      glm::value_ptr(controls.camera_translation), -700.0f,
                      700.0f);
  ImGui::SliderAngle("Camera rotation X", &controls.camera_rotation.x, -180.0f,
                     180.0f);
  ImGui::SliderAngle("Camera rotation Y", &controls.camera_rotation.y, -180.0f,
                     180.0f);
  ImGui::SliderAngle("Camera rotation Z", &controls.camera_rotation.z, -180.0f,
                     180.0f);
  ImGui::SliderAngle("Field Of View Y", &controls.fovy, 1.0f, 90.0f);
  ImGui::SliderFloat("Aspect Ratio", &controls.aspect_ratio, 5 * ASPECT_RATIO,
                     ASPECT_RATIO / 5);
  ImGui::ColorEdit3("Triangle color", glm::value_ptr(controls.object_colors));
  ImGui::Checkbox("Look at object", &controls.look_at_object);
  ImGui::Checkbox("Spin object", &controls.spin_object);
  if (ImGui::Button("reset")) {
    controls = Controls();
  }

  // Lights
  ImGui::ColorEdit3("Light color", glm::value_ptr(controls.light_color));
  ImGui::SliderFloat3("Light position",
                      glm::value_ptr(controls.light_translation), -700.0f,
                      700.0f);

  // misc
  ImGui::RadioButton("CW polygons", (int *)&controls.orientation_cw, 1);
  ImGui::SameLine();
  ImGui::RadioButton("CCW polygons", (int *)&controls.orientation_cw, 0);

  // misc
  ImGui::Text("Render");
  ImGui::RadioButton("Points", (int *)&controls.chosen_render, POINTS);
  ImGui::SameLine();
  ImGui::RadioButton("Wireframe", (int *)&controls.chosen_render, WIREFRAME);
  ImGui::SameLine();
  ImGui::RadioButton("Solid", (int *)&controls.chosen_render, SOLID);

  if (controls.orientation_cw == 0) {
    glFrontFace(GL_CCW);
  } else if (controls.orientation_cw == 1) {
    glFrontFace(GL_CW);
  }

  ImGui::Text("Shader Type");
  ImGui::RadioButton("Phong", (int *)&controls.chosen_shader, PHONG);
  ImGui::RadioButton("Resolution", (int *)&controls.chosen_shader, RESOLUTION);
  ImGui::RadioButton("'Represented' Triangles", (int *)&controls.chosen_shader,
                     RESOLUTION_THRESHOLD);

  ImGui::SliderFloat("Near Clip", &controls.near_clip, 0.0f, 5.0f);
  ImGui::SliderFloat("Far Clip", &controls.far_clip, 5.0f, 5000.0f);

  ImGui::Text("FPS: %.3f ms/frame (%.1f FPS)", controls.frame_time_opengl,
              1000.0 / controls.frame_time_opengl);

  controls.resolution_threshold = glm::round(controls.resolution_threshold);
}

void setupGUI(GLFWwindow *window) {
  /** Setup imgui
   */
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();
}

void clearGUI() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void drawGUI() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
