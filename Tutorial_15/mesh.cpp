#include "mesh.h"
#include "material.h"

GLuint Mesh::m_elementBuffer = 0;

Mesh::Mesh()
    : m_material(0),
      m_node(0)

{
}

void Mesh::draw(Scene* scene, Material* material)
{

    QOpenGLFunctions *gl = QOpenGLContext::currentContext()->functions();

    if (!m_elementBuffer) {
        gl->glGenBuffers(1, &m_elementBuffer);
    }

    if (material == NULL) {
        material = m_material;
    }

    if (material != NULL) {
        // activate
        material->activate(scene, this);

        gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
        gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);
        gl->glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, (void*)0);

        material->release();
    }
}
