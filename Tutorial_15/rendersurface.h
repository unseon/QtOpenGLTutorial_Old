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

class Scene;

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

    float m_distance;
    float m_rotationX;
    float m_rotationY;

    GLuint m_frameBuffer;
    GLuint m_renderedTextureId;
    GLuint m_depthRenderBuffer;

    QOpenGLTexture* m_renderedTexture;

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
    void initializeScene();
    void initializeSurface();

    QImage m_surface;
    uchar *m_data;

    Scene* m_scene;
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
