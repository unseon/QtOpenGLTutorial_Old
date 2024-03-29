#ifndef RENDERSURFACE_H
#define RENDERSURFACE_H

#include <QQuickItem>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QTimer>
#include <QTime>
#include <QElapsedTimer>
#include <QQuickPaintedItem>

#include <vector>

using std::vector;

class Renderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    Renderer();
    ~Renderer();

    void setViewportSize(const QSize &size);
public slots:
    void render();

private:
    bool initialized;

    QSize m_viewportSize;
    QQuickWindow *m_window;

    QOpenGLShaderProgram *m_program;
    QOpenGLShaderProgram *m_surfaceProgram;

    QOpenGLTexture *m_texture;
    QOpenGLTexture *m_normalmap;

    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;
    QMatrix4x4 m_model;

    float m_distance;
    float m_rotationX;
    float m_rotationY;

    GLuint m_elementBuffer;

    float *m_vertexBufferData;
    float *m_uvBufferData;
    unsigned int *m_indexBufferData;

    unsigned int m_sizeVertices;
    unsigned int m_sizeUv;
    unsigned int m_sizeIndices;

    vector<float> m_vertices;
    vector<float> m_uvs;
    vector<unsigned int> m_indices;
    vector<float> m_normals;
    vector<float> m_tangents;
    vector<float> m_bitangents;

    QString m_textureFile;
    QString m_normalmapFile;

    QSize m_size;

    GLuint m_frameBuffer;
    GLuint m_renderedTexture;
    GLuint m_depthRenderBuffer;

    bool m_isViewportDirty;

    void prepareRender();

public:
    float camRotationX();
    float camRotationY();
    float camDistance();

    void setCamRotationX(float value);
    void setCamRotationY(float value);
    void setCamDistance(float value);

    void updateCamera();
    void initializeMesh();
    void initializeSurface();

    QImage m_surface;
    uchar *m_data;
};

class RenderSurface : public QQuickPaintedItem
{
    Q_OBJECT

public:
    RenderSurface(QQuickItem* parent = 0);
    Q_PROPERTY(float fps MEMBER m_fps NOTIFY fpsChanged)

signals:
    void fpsChanged();

public slots:
    void cleanup();

private slots:
    void tick();

private:
    Renderer *m_renderer;
    bool m_leftMouseDown;
    QPointF m_originPos;
    float m_originRotationX;
    float m_originRotationY;

    QTimer* m_loop;
    QElapsedTimer time;

    float m_fps;

    bool m_upKey;
    bool m_downKey;
    bool m_leftKey;
    bool m_rightKey;

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void paint(QPainter *painter);
};

#endif // RENDERSURFACE_H
