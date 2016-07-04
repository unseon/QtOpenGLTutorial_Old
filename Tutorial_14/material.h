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

    QString m_textureFile;
    QString m_normalmapFile;

    QString m_vertexShaderPath;
    QString m_fragmentShaderPath;

    QOpenGLTexture *m_texture;
    QOpenGLTexture *m_normalmap;

    float m_shininess;
    float m_opacity;

    QOpenGLShaderProgram *m_program;

    void init();

    void activate(Scene* scene);
    void release();
};

#endif // MATERIAL_H
