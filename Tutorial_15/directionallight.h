#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include <QVector4D>

class Node;

class DirectionalLight
{
public:
    DirectionalLight();

    Node* m_node;

    QVector4D m_color;
};

#endif // DIRECTIONALLIGHT_H
