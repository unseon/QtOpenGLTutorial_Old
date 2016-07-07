#include "node.h"

#include "mesh.h"
#include "scene.h"


Node::Node(Node* parent)
    :m_mesh(0)
{
    m_parent = parent;
}

void Node::addChild(Node* node)
{
    m_children.push_back(node);
    node->m_parent = this;
}

void Node::render(Scene* scene)
{
    renderSelf(scene);

    renderChildren(scene);
}

void Node::renderSelf(Scene* scene)
{
    updateMatrix();

    if (m_mesh != NULL) {
        m_mesh->draw(scene);
    }
}

void Node::renderChildren(Scene* scene)
{
    for(int i = 0; i < m_children.size(); i++) {
        m_children[i]->render(scene);
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
    mat.rotate(m_rotation.x(), 1.0f, 0.0f, 0.0f);
    mat.rotate(m_rotation.y(), 0.0f, 1.0f, 0.0f);
    mat.rotate(m_rotation.z(), 0.0f, 0.0f, 1.0f);
    mat.scale(m_scale);

    m_transform = mat;
}
