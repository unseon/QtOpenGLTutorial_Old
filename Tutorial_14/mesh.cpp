#include "mesh.h"
#include "material.h"

Mesh::Mesh(Node* node)
    :m_material(0)
{
    m_node = node;
}

void Mesh::draw(Scene* scene)
{


    if (!m_material) {
        // activate
        m_material->activate(scene);

        // draw element

        // release
        m_material->release();
    }
}
