#ifndef RENDERSURFACE_H
#define RENDERSURFACE_H

#include <QQuickItem>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QTimer>
#include <QTime>
#include <QElapsedTimer>

class Renderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    Renderer();
    ~Renderer();

    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void setWindow(QQuickWindow *window) { m_window = window; }

public slots:
    void render();

private:
    bool initialized;

    QSize m_viewportSize;
    QQuickWindow *m_window;
    QOpenGLShaderProgram *m_program;
    QOpenGLTexture *m_texture;

    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;
    QMatrix4x4 m_model;

    float m_distance;
    float m_rotationX;
    float m_rotationY;

    float *m_vertexBufferData;
    float *m_uvBufferData;


public:
    float camRotationX();
    float camRotationY();
    float camDistance();

    void setCamRotationX(float value);
    void setCamRotationY(float value);
    void setCamDistance(float value);

    void updateCamera();
    void initializeMesh();
};

class RenderSurface : public QQuickItem
{
    Q_OBJECT
public:
    RenderSurface();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);
    void tick();

private:
    Renderer *m_renderer;
    bool m_leftMouseDown;
    QPointF m_originPos;
    float m_originRotationX;
    float m_originRotationY;

    QTimer* m_loop;
    QElapsedTimer time;

    bool m_upKey;
    bool m_downKey;
    bool m_leftKey;
    bool m_rightKey;

    void updateWindow();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // RENDERSURFACE_H
