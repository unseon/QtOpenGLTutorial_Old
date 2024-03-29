#include "node.h"

#include "mesh.h"
#include "scene.h"
#include "material.h"
#include "directionallight.h"


Node::Node(Node* parent)
    :m_mesh(0),
      m_light(0),
      m_position(0.0f, 0.0f, 0.0f),
      m_rotation(0.0f, 0.0f, 0.0f),
      m_scale(1.0f, 1.0f, 1.0f),
      m_rotationPivot(0.0f, 0.0f, 0.0f)
{
    m_parent = parent;
}

void Node::addChild(Node* node)
{
    m_children.push_back(node);
    node->m_parent = this;
}

void Node::render(Scene* scene, Material* material)
{
    renderSelf(scene, material);

    renderChildren(scene, material);
}

void Node::renderSelf(Scene* scene, Material* material)
{
    updateMatrix();

    if (m_mesh != NULL) {
        m_mesh->draw(scene, material);
    }
}

void Node::renderChildren(Scene* scene, Material* material)
{
    for(int i = 0; i < m_children.size(); i++) {
        m_children[i]->render(scene, material);
    }
}

QMatrix4x4 Node::netMatrix()
{
    updateMatrix();

    if (m_parent == NULL) {
        return m_transform;
    } else {
        return m_parent->netMatrix() * m_transform;
    }
}

void Node::updateMatrix()
{
    QMatrix4x4 mat;
    mat.setToIdentity();

    mat.translate(m_position);

    mat.rotate(m_rotation.z(), 0.0f, 0.0f, 1.0f);
    mat.rotate(m_rotation.y(), 0.0f, 1.0f, 0.0f);
    mat.rotate(m_rotation.x(), 1.0f, 0.0f, 0.0f);

    mat.rotate(m_rotationPivot.z(), 1.0f, 0.0f, 0.0f);
    mat.rotate(m_rotationPivot.y(), 1.0f, 0.0f, 0.0f);
    mat.rotate(m_rotationPivot.x(), 1.0f, 0.0f, 0.0f);

    mat.scale(m_scale);

    m_transform = mat;
}
