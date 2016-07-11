#include "scene.h"

#include "node.h"
#include "material.h"
#include "directionallight.h"

Scene::Scene()
    : m_root(0),
    m_mainLight(0)
{
    m_backgroundColor = QVector4D(0.0f, 0.0f, 0.0f, 1.0f);
    m_lightAmbientColor = QVector4D(0.1f, 0.1f, 0.1f, 1.0f);
    //m_lightDiffuseColor = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);
    m_lightSpecularColor = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);

    m_lightDirWorld = QVector4D (1, -1, -1, 0);
    m_lightPosition = QVector4D (-10, 10, 10, 0);
}

void Scene::render(Material* material)
{
    if (m_root) {
//        m_viewMatrix = viewMatrix;
//        m_projectionMatrix = projectionMatrix;
        m_root->render(this, material);
    }
}
