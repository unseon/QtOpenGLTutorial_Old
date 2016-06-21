#ifndef SCENE_H
#define SCENE_H

#include <QVector4D>
#include <QMatrix4x4>
#include <QColor>

class Node;

class Scene
{
public:
    Scene();

    Node* m_root;

    QColor m_lightAmbientColor;
    QColor m_lightSpecularColor;
    QVector4D m_lightDirWorld;

    QColor m_backgroundColor;

    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_viewMatrix;
};

#endif // SCENE_H
