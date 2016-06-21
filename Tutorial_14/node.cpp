#include "node.h"

Node::Node(Node* parent)
{
    m_parent = parent;
}

void Node::render(Scene* scene)
{

}

void Node::renderSelf(Scene* scene)
{

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

    if (!m_parent) {
        return m_transform;
    } else {
        return m_parent->netMatrix() * m_transform;
    }
}

void Node::updateMatrix()
{
    QMatrix4x4 mat;
    mat.setToIdentity();

    mat.scale(m_scale);
    mat.rotate(m_rotation.x(), 1.0f, 0.0f, 0.0f);
    mat.rotate(m_rotation.y(), 0.0f, 1.0f, 0.0f);
    mat.rotate(m_rotation.z(), 0.0f, 0.0f, 1.0f);
    mat.translate(m_position);

    m_transform = mat;
}
