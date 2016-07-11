#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include <QVector4D>
#include <QOpenGLFunctions>

class Node;

class DirectionalLight
{
public:
    DirectionalLight();

    Node* m_node;

    QVector4D m_color;

    GLuint m_shadowMapFrameBuffer;
    GLuint m_shadowMapTexture;
    GLuint m_shadowMapDepthBuffer;

    void prepare();
};

#endif // DIRECTIONALLIGHT_H
