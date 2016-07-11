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
    QMatrix4x4 viewMatrix = m_viewMatrix;
    QMatrix4x4 projectionMatrix = m_projectionMatrix;

    if (m_mainLight) {
        QVector4D forward(0, -1, 0, 0);

        QVector4D forwardPosition(0, -1, 0, 1);
        QVector3D up(0, 1, 0);

        QMatrix4x4 localToWorld = m_mainLight->netMatrix();
        QMatrix4x4 normalWorld = localToWorld.inverted().transposed();

        QVector3D viewPoint = localToWorld * QVector3D(0, 0, 0);
        //QVector4D viewForward = QVector4D((normalWorld * forward).toVector3D()).normalized();
        QVector4D viewForward = normalWorld * forward;
        //QVector3D viewTarget = (localToWorld * forwardPosition).toVector3D();
        //QVector3D viewTarget(0, 0, 0);
        QVector3D viewTarget = viewPoint + viewForward.toVector3D();
        QVector3D viewUp = up;

        QMatrix4x4 lightViewMatrix;

        lightViewMatrix.setToIdentity();
        lightViewMatrix.lookAt(viewPoint,
                               viewTarget,
                               viewUp);

        m_viewMatrix = lightViewMatrix;

        m_projectionMatrix.setToIdentity();
        m_projectionMatrix.ortho(-20, 20, -20, 20, -60, 60);
    }

    if (m_root) {
//        m_viewMatrix = viewMatrix;
//        m_projectionMatrix = projectionMatrix;
        m_root->render(this, material);
    }
}
