#include "material.h"

#include <QOpenGLShaderProgram>

GLuint Material::m_elementBuffer = 0;

Material::Material(Mesh* mesh)
    : m_program(0)
{
    m_mesh = mesh;
}

void Material::init()
{
    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/basic_shading_04.vs");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/basic_shading_04.fs");

    m_program->bindAttributeLocation("vertices", 0);
    m_program->bindAttributeLocation("texCoord", 1);
    m_program->bindAttributeLocation("normals", 2);
    m_program->bindAttributeLocation("tangents", 3);
    m_program->bindAttributeLocation("bitangents", 4);
    m_program->link();

    m_texture = new QOpenGLTexture(QImage(m_textureFile).mirrored());
    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    m_texture->setMagnificationFilter(QOpenGLTexture::Nearest);

    m_normalmap = new QOpenGLTexture(QImage(m_normalmapFile).mirrored());
    m_normalmap->setMinificationFilter(QOpenGLTexture::Nearest);
    m_normalmap->setMagnificationFilter(QOpenGLTexture::Nearest);
}


void Material::activate(Scene* scene)
{
    if (!m_program) {
        init();
    }

    if (!m_elementBuffer) {
        glGenBuffers(1, &m_elementBuffer);
    }

    m_program->bind();

    m_program->setUniformValue("texture", 0);
    m_texture->bind(0);

    m_program->setUniformValue("normalmap", 1);
    m_normalmap->bind(1);

    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(1);
    m_program->enableAttributeArray(2);
    m_program->enableAttributeArray(3);
    m_program->enableAttributeArray(4);

    m_program->setAttributeArray(0, GL_FLOAT, &m_mesh->m_vertices[0], 3);
    m_program->setAttributeArray(1, GL_FLOAT, &m_mesh->m_uvs[0], 2);
    m_program->setAttributeArray(2, GL_FLOAT, &m_mesh->m_normals[0], 3);
    m_program->setAttributeArray(3, GL_FLOAT, &m_mesh->m_tangents[0], 3);
    m_program->setAttributeArray(4, GL_FLOAT, &m_mesh->m_bitangents[0], 3);

    QMatrix4x4 model = m_mesh->m_node->netMatrix();
    QMatrix4x4 view = scene->m_viewMatrix;
    QMatrix4x4 projection = scene->m_viewMatrix;

    QMatrix4x4 mvp = projection * view * model;
    QMatrix4x4 mv = view * model;
    QMatrix4x4 normalMatrix = mv.inverted().transposed();
    QMatrix4x4 lightNormalMatrix = view.inverted().transposed();

    QVector4D lightDirWorld = scene->m_lightDirWorld.normalized();
    QVector4D lightDirView = lightDirWorld * normalMatrix;

    QVector4D sceneBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f);
    QVector4D lightAmbientColor(0.1f, 0.1f, 0.1f, 1.0f);
    QVector4D lightDiffuseColor(0.8f, 0.8f, 0.8f, 1.0f);
    QVector4D lightSpecularColor(0.8f, 0.8f, 0.8f, 1.0f);
    float materialShininess = 0.5f;
    float materialOpacity = 1.0f;

    m_program->setUniformValue("MVP", mvp);
    m_program->setUniformValue("MV", mv);
    m_program->setUniformValue("NormalMatrix", normalMatrix);

    m_program->setUniformValue("light.ambient", scene->m_lightAmbientColor);
    m_program->setUniformValue("light.specular", scene->m_lightSpecularColor);
    m_program->setUniformValue("material.shininess", m_shininess);
    m_program->setUniformValue("material.opacity", m_opacity);

    m_program->setUniformValue("light.direction", lightDirView);

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

void Material::draw()
{
    QOpenGLFunctions *gl = QOpenGLContext::currentContext()->functions();
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
    gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mesh->m_indices.size() * sizeof(unsigned int), m_mesh->m_indices.data(), GL_STATIC_DRAW);
    gl->glDrawElements(GL_TRIANGLES, m_mesh->m_indices.size(), GL_UNSIGNED_INT, (void*)0);
}
