#ifndef SCENE_H
#define SCENE_H

#include <QVector4D>
#include <QMatrix4x4>
#include <QColor>
#include <QVector4D>

class Node;
class Material;

class Scene
{
public:
    Scene();

    Node* m_root;

    QVector4D m_lightAmbientColor;
    QVector4D m_lightSpecularColor;
    QVector4D m_lightDirWorld;

    QVector4D m_backgroundColor;

    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_modelMatrix;

    void render(Material* mateiral = 0);
};

#endif // SCENE_H
