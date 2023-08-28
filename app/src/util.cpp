#include "util.h"
#include "Debug.h"
#include <glm/common.hpp>

static const GLchar *ReadShader(const char *filename) {
  FILE *infile = fopen(filename, "rb");

  if (!infile) {
    std::cerr << "Unable to open file '" << filename << "'" << std::endl;
    return NULL;
  }

  fseek(infile, 0, SEEK_END);
  int len = ftell(infile);
  fseek(infile, 0, SEEK_SET);

  GLchar *source = new GLchar[len + 1];

  fread(source, 1, len, infile);
  fclose(infile);

  source[len] = 0;

  return const_cast<const GLchar *>(source);
}

GLuint LoadShaders(ShaderInfo *shaders) {
  if (shaders == NULL) {
    return 0;
  }

  GLuint program = glCreateProgram();

  ShaderInfo *entry = shaders;
  while (entry->type != GL_NONE) {
    GLuint shader = glCreateShader(entry->type);

    entry->shader = shader;

    const GLchar *source = ReadShader(entry->filename);
    if (source == NULL) {
      for (entry = shaders; entry->type != GL_NONE; ++entry) {
        glDeleteShader(entry->shader);
        entry->shader = 0;
      }

      return 0;
    }

    GLCall(glShaderSource(shader, 1, &source, NULL));
    delete[] source;

    GLCall(glCompileShader(shader));

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
      GLsizei len;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

      GLchar *log = new GLchar[len + 1];
      glGetShaderInfoLog(shader, len, &len, log);
      std::cerr << "Shader compilation failed: " << log << std::endl;
      delete[] log;

      return 0;
    }

    GLCall(glAttachShader(program, shader));
    std::cout << "Compiled shaders correctly " << std::endl;

    ++entry;
  }

  glLinkProgram(program);

  GLint linked;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if (!linked) {
    GLsizei len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

    GLchar *log = new GLchar[len + 1];
    glGetProgramInfoLog(program, len, &len, log);
    std::cerr << "Shader linking failed: " << log << std::endl;
    delete[] log;

    for (entry = shaders; entry->type != GL_NONE; ++entry) {
      glDeleteShader(entry->shader);
      entry->shader = 0;
    }

    return 0;
  }

  return program;
}

BoundingBox calculate_bbox(Vertex *vertices, GLuint NumVertices) {

  BoundingBox bbox;

  for (int i = 0; i < NumVertices; i++) {
    glm::vec3 v;
    v.x = vertices[i].coordinate[0];
    v.y = vertices[i].coordinate[1];
    v.z = vertices[i].coordinate[2];

    bbox.max = glm::max(v, bbox.max);
    bbox.min = glm::min(v, bbox.min);
  }

  return bbox;
}
