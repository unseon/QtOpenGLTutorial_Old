#include "rendersurface.h"

#include <QQuickWindow>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>

RenderSurface::RenderSurface()
    : m_renderer(0)
{
    connect(this, &QQuickItem::windowChanged, this, &RenderSurface::handleWindowChanged);
}

void RenderSurface::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &RenderSurface::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &RenderSurface::cleanup, Qt::DirectConnection);

        win->setClearBeforeRendering(false);
    }
}

void RenderSurface::cleanup()
{
    if (m_renderer) {
        delete m_renderer;
        m_renderer = 0;
    }
}

void RenderSurface::sync()
{
    if (!m_renderer) {
        m_renderer = new Renderer();
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &Renderer::render, Qt::DirectConnection);
    }

    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setWindow(window());
}

Renderer::Renderer()
    :m_program(0)
{
    initializeOpenGLFunctions();
}

Renderer::~Renderer()
{
}

void Renderer::render()
{
    if (!m_program) {
        m_program = new QOpenGLShaderProgram();
        m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                           "attribute highp vec4 vertices;"
                                           "uniform mat4 MVP;"
                                           "void main() {"
                                           "    gl_Position = MVP * vertices;"
                                           "}");
        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                           "uniform vec3 color;"
                                           "void main() {"
                                           "    gl_FragColor = vec4(color.xyz, 1.0);"
                                           "}");

        m_program->bindAttributeLocation("vertices", 0);
        m_program->link();
    }

    m_program->bind();

    m_program->enableAttributeArray(0);

    float values[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
    };
    m_program->setAttributeArray(0, GL_FLOAT, values, 3);

    // set red color
    float color[] = {1.0f, 0.0f, 0.0f};
    m_program->setUniformValueArray("color", color, 1, 3);

    QMatrix4x4 projection;
    QMatrix4x4 view;
    QMatrix4x4 model;

    projection.perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    view.lookAt(QVector3D(4.0f, 3.0f, 3.0f),
                 QVector3D(0.0f, 0.0f, 0.0f),
                 QVector3D(0.0f, 1.0f, 0.0f));

    model.setToIdentity();

    QMatrix4x4 mvp = projection * view * model;
    m_program->setUniformValue("MVP", mvp);

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    glDisable(GL_DEPTH_TEST);

    glClearColor(0, 0, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    m_program->disableAttributeArray(0);
    m_program->release();

    m_window->resetOpenGLState();
}
