#ifndef RENDERSURFACE_H
#define RENDERSURFACE_H

#include <QQuickItem>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>

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

    void updateCamera();
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

private:
    Renderer *m_renderer;
    bool m_leftMouseDown;
    QPointF m_originPos;

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
};

#endif // RENDERSURFACE_H
