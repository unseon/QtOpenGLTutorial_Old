#include "material.h"

#include <QOpenGLShaderProgram>

#include "scene.h"
#include "node.h"
#include "mesh.h"
#include "directionallight.h"

Material::Material(const QString &shaderName)
    : m_program(0),
      m_texture(0),
      m_normalmap(0),
      m_shaderName(shaderName)
{
    m_shininess = 0.5f;
    m_opacity = 1.0f;
    m_diffuse = QVector4D(0.5f, 0.5f, 0.5f, 1.0f);
    m_specular = QVector4D(0.8f, 0.8f, 0.8f, 1.0f);

    //m_shaderName = "solid_color_shading";
    //m_shaderName = "normal_map_shading";
    //m_shaderName = "texture_shading";
}

void Material::init()
{
    if (m_shaderName.isEmpty()) {

        if (!m_textureFile.isEmpty() && !m_normalmapFile.isEmpty()) {
            m_shaderName = "normal_map_shading";

            m_texture = new QOpenGLTexture(QImage(m_textureFile).mirrored());
            m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
            m_texture->setMagnificationFilter(QOpenGLTexture::Nearest);

            m_normalmap = new QOpenGLTexture(QImage(m_normalmapFile).mirrored());
            m_normalmap->setMinificationFilter(QOpenGLTexture::Nearest);
            m_normalmap->setMagnificationFilter(QOpenGLTexture::Nearest);
        } else if (!m_textureFile.isEmpty()) {
            m_shaderName = "texture_shading";

            m_texture = new QOpenGLTexture(QImage(m_textureFile).mirrored());
            m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
            m_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
        } else {
            m_shaderName = "solid_color_shadow_shading";

            m_texture = new QOpenGLTexture(QImage("uvtemplate.png").mirrored());
            m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
            m_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
        }
    }

    QString vertexShaderPath = m_shaderName;
    vertexShaderPath.prepend(":/shaders/").append(".vs");

    QString fragmentShaderPath = m_shaderName;
    fragmentShaderPath.prepend(":/shaders/").append(".fs");

    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderPath);
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderPath);

    m_program->bindAttributeLocation("vertices", 0);
    m_program->bindAttributeLocation("texCoord", 1);
    m_program->bindAttributeLocation("normals", 2);
    m_program->bindAttributeLocation("tangents", 3);
    m_program->bindAttributeLocation("bitangents", 4);
    m_program->link();
}


void Material::activate(Scene* scene, Mesh* mesh)
{
    if (m_program == NULL) {
        init();
    }

    m_program->bind();

    if (!m_textureFile.isEmpty()) {
        m_program->setUniformValue("texture", 0);
        m_texture->bind(0);
    }

    if (!m_normalmapFile.isEmpty()) {
        m_program->setUniformValue("normalmap", 1);
        m_normalmap->bind(1);
    }

    m_program->enableAttributeArray(0);
    m_program->setAttributeArray(0, GL_FLOAT, &mesh->m_vertices[0], 3);

    if (!mesh->m_uvs.empty()) {
        m_program->enableAttributeArray(1);
        m_program->setAttributeArray(1, GL_FLOAT, &mesh->m_uvs[0], 2);
    }

    if (!mesh->m_normals.empty()) {
        m_program->enableAttributeArray(2);
        m_program->setAttributeArray(2, GL_FLOAT, &mesh->m_normals[0], 3);
    }

    if (!mesh->m_tangents.empty()) {
        m_program->enableAttributeArray(3);
        m_program->setAttributeArray(3, GL_FLOAT, &mesh->m_tangents[0], 3);
    }

    if (!mesh->m_bitangents.empty()) {
        m_program->enableAttributeArray(4);
        m_program->setAttributeArray(4, GL_FLOAT, &mesh->m_bitangents[0], 3);
    }

    QMatrix4x4 localToWorld =  scene->m_modelMatrix * mesh->m_node->netMatrix();
    QMatrix4x4 model = scene->m_modelMatrix;
    //QMatrix4x4 model = scene->m_modelMatrix;
    QMatrix4x4 view = scene->m_viewMatrix;
    QMatrix4x4 projection = scene->m_projectionMatrix;

    QMatrix4x4 mvp = projection * view * localToWorld;
    QMatrix4x4 mv = view * localToWorld;
    QMatrix4x4 normalMatrix = mv.inverted().transposed();

    QVector4D lightDirWorld = scene->m_lightDirWorld.normalized();
    QVector4D lightDirView = (model * lightDirWorld).normalized();

    m_program->setUniformValue("MVP", mvp);
    m_program->setUniformValue("MV", mv);
    m_program->setUniformValue("NormalMatrix", normalMatrix);

    m_program->setUniformValue("light.ambient", scene->m_lightAmbientColor);
    m_program->setUniformValue("light.specular", scene->m_lightSpecularColor);

    //m_program->setUniformValue("material.emission", m_emission);
    //m_program->setUniformValue("material.ambient", m_ambient);
    m_program->setUniformValue("material.diffuse", m_diffuse);
    m_program->setUniformValue("material.specular", m_specular);

    m_program->setUniformValue("material.shininess", m_shininess);
    m_program->setUniformValue("material.opacity", m_opacity);

    m_program->setUniformValue("light.direction", QVector3D(lightDirView).normalized());


    DirectionalLight* light = scene->m_mainLight->m_light;
    if (light->m_shadowMapTexture > 0 && m_shaderName == "texture_shading") {
        QOpenGLFunctions *gl = QOpenGLContext::currentContext()->functions();
        glActiveTexture(GL_TEXTURE2);
        gl->glBindTexture(GL_TEXTURE_2D, light->m_shadowMapTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        m_program->setUniformValue("shadowmap", 2);

        QVector4D forward(0, 0, -1, 0);

        QVector3D up(0, 1, 0);
        QMatrix4x4 localToWorld = scene->m_mainLight->netMatrix();
        QMatrix4x4 normalWorld = localToWorld.inverted().transposed();

        QVector3D viewPoint = localToWorld * QVector3D(0, 0, 0);
        QVector4D viewForward = normalWorld * forward;
        QVector3D viewTarget = viewPoint + viewForward.toVector3D();
        QVector3D viewUp = normalWorld * up;

        QMatrix4x4 lightViewMatrix;

        lightViewMatrix.setToIdentity();
        lightViewMatrix.lookAt(viewPoint,
                               viewTarget,
                               viewUp.normalized());

        QMatrix4x4 lightProjection;
        lightProjection.setToIdentity();
        lightProjection.ortho(-20, 20, -20, 20, -60, 60);

        QMatrix4x4 lightMVP = lightProjection * lightViewMatrix * mesh->m_node->netMatrix();

        m_program->setUniformValue("light.view", lightViewMatrix);
        m_program->setUniformValue("light.projection", lightProjection);
        m_program->setUniformValue("Model", model);
        m_program->setUniformValue("lightMVP", lightMVP);



    }

    m_program->setUniformValue("scene.backgroundColor", scene->m_backgroundColor);
}

void Material::release()
{
    m_program->disableAttributeArray(0);
    m_program->disableAttributeArray(1);
    m_program->disableAttributeArray(2);
    m_program->disableAttributeArray(3);
    m_program->disableAttributeArray(4);
    m_program->release();
}


