#include "VertexBuffer.h"
#include "Debug.h"

VertexBuffer::VertexBuffer(const void *data, unsigned int size,
                           unsigned int vertexCount) {
  GLCall(glGenBuffers(1, &m_RendererID));
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
  GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
  this->m_vertexCount = vertexCount;
}

VertexBuffer::~VertexBuffer() { GLCall(glDeleteBuffers(1, &m_RendererID)); }

void VertexBuffer::Bind() const {
  GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind() const { GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); }

unsigned int VertexBuffer::getCount() { return this->m_vertexCount; }
