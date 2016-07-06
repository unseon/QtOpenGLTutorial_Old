#include "scene.h"

#include "node.h"

Scene::Scene()
    : m_root(0)
{
    m_backgroundColor = QVector4D(0.0f, 0.0f, 0.0f, 1.0f);
    m_lightAmbientColor = QVector4D(0.1f, 0.1f, 0.1f, 1.0f);
    //m_lightDiffuseColor = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);
    m_lightSpecularColor = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);

    m_lightDirWorld = QVector4D (0, 0, -1, 0);
}

void Scene::render()
{
    if (m_root) {
        m_root->render(this);
    }
}
