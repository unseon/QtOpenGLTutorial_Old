#include "scene.h"

#include "node.h"

Scene::Scene()
    : m_root(0)
{
    m_backgroundColor = QColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_lightAmbientColor = QColor(0.1f, 0.1f, 0.1f, 1.0f);
    //m_lightDiffuseColor = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);
    m_lightSpecularColor = QColor(0.8f, 0.8f, 0.8f, 1.0f);

    m_lightDirWorld = QVector4D (1, -1, -1, 0);
}

void Scene::render()
{
    if (m_root) {
        m_root->render(this);
    }
}
