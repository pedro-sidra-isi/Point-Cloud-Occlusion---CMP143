#pragma once

class VertexBuffer {
private:
  unsigned int m_RendererID;
  unsigned int m_vertexCount;

public:
  VertexBuffer(const void *data, unsigned int size, unsigned int vertexCount);
  ~VertexBuffer();

  void Bind() const;
  void Unbind() const;

  unsigned int getCount();
};
