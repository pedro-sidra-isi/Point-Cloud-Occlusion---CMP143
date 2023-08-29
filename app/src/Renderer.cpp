#include "Renderer.h"
#include "Object.h"
#include "util.h"
#include <iostream>

Renderer::Renderer() { glEnable(GL_DEPTH_TEST); }

void Renderer::Clear() const {
  GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray &va, const Shader &shader,
                    unsigned int numVerts, GLuint type) const {
  GLCall(glEnable(GL_CULL_FACE));
  shader.Bind();
  va.Bind();
  glDrawArrays(type, 0, numVerts);
}

void Renderer::Draw(const Object &object, const Shader &shader,
                    GLuint type) const {
  GLCall(glEnable(GL_CULL_FACE));
  shader.Bind();
  object.VA.Bind();
  glDrawArrays(type, 0, object.VB->getCount());
}

void Renderer::Draw(const PlyObject &object, const Shader &shader,
                    GLuint type) const {
  GLCall(glEnable(GL_CULL_FACE));
  shader.Bind();
  object.VA.Bind();
  object.IB->Bind();
  glDrawElements(GL_TRIANGLES, object.IB->GetCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::Draw(const PlyPointCloud &object, const Shader &shader) const {
  GLCall(glEnable(GL_CULL_FACE));
  shader.Bind();
  object.VA.Bind();
  glDrawArrays(GL_POINTS, 0, object.VB->getCount());
}

void Renderer::DrawFullscreenImage(FullImageBuffer &src, Shader &shader,
                                   unsigned char *image) {
  src.draw(image);
  shader.SetUniform1i("texture1", src.imageTexture);

  shader.Bind();
  src.VA.Bind();
  GLCall(glDrawArrays(GL_TRIANGLES, 0, src.VB->getCount()));
}
