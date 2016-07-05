#ifndef MATERIAL_H
#define MATERIAL_H

#include <QString>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include "node.h"
#include "mesh.h"

class Material
{
public:
    Material();

    Mesh* m_mesh;

    QVector4D m_diffuseColor;

    QString m_textureFile;
    QString m_normalmapFile;

    QString m_shaderName;

    QString m_vertexShaderPath;
    QString m_fragmentShaderPath;

    QOpenGLTexture *m_texture;
    QOpenGLTexture *m_normalmap;

    QVector4D m_emission;
    QVector4D m_ambient;
    QVector4D m_diffuse;
    QVector4D m_specular;

    float m_shininess;
    float m_opacity;

    QOpenGLShaderProgram *m_program;

    void init();

    void activate(Scene* scene);
    void release();
};

#endif // MATERIAL_H
