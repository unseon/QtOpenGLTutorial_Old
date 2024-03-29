#ifndef NODE_H
#define NODE_H

#include <QMatrix4x4>
#include <QVector3D>
#include <vector>
#include <QOpenGLFunctions>


class Mesh;
class Scene;

using std::vector;

class Node
{
public:
    Node(Node* parent = 0);

    QString m_name;

    QVector3D m_position;
    QVector3D m_rotation;
    QVector3D m_scale;

    QMatrix4x4 m_transform;

    Node* m_parent;
    vector<Node*> m_children;

    Mesh* m_mesh;

    void render(Scene* scene);
    QMatrix4x4 netMatrix();

    void updateMatrix();
    void addChild(Node* node);

protected:
    void renderSelf(Scene* scene);
    void renderChildren(Scene* scene);
};

#endif // NODE_H
