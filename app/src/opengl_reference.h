#pragma once

#include "Debug.h"
#include "GL/glcorearb.h"
#include "IndexBuffer.h"
#include "Object.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "util.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL3/gl3.h>
#include <GL3/gl3w.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>

// Controls the shader
// for init'ing opengl windows
GLFWwindow *init_GL(std::string window_name);

void setUniforms_PhongGouraud(Shader *use_shader, Object &giselle,
                              glm::mat4 Model, glm::mat4 View,
                              glm::mat4 Projection, Light light);

// GUI Functions
void updateGUI(Controls &controls);
void setupGUI(GLFWwindow *window);
void clearGUI();
void drawGUI();

// Main function, with rendering loop
void opengl(Controls *controls);
