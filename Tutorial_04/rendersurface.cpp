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
                                           "attribute highp vec3 vertexColor;"
                                           "varying vec3 fragmentColor;"
                                           "uniform mat4 MVP;"
                                           "void main() {"
                                           "    gl_Position = MVP * vertices;"
                                           "    fragmentColor = vertexColor;"
                                           "}");
        m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                           "varying vec3 fragmentColor;"
                                           "void main() {"
                                           "    gl_FragColor = vec4(fragmentColor, 1.0);"
                                           "}");

        m_program->bindAttributeLocation("vertices", 0);
        m_program->bindAttributeLocation("vertexColor", 1);
        m_program->link();
    }

    m_program->bind();

    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(1);

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,

         1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,

         1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,

         1.0f, 1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,

        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,

         1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,

        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,

         1.0f, 1.0f, 1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f,-1.0f,

         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,

         1.0f, 1.0f, 1.0f,
         1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,

         1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,

         1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 1.0f
    };

    // One color for each vertex. They were generated randomly.
    static const GLfloat g_color_buffer_data[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };

    m_program->setAttributeArray(0, GL_FLOAT, g_vertex_buffer_data, 3);
    m_program->setAttributeArray(1, GL_FLOAT, g_color_buffer_data, 3);

    QMatrix4x4 projection;
    QMatrix4x4 view;
    QMatrix4x4 model;

    projection.perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    view.lookAt(QVector3D(3.0f, 3.0f, 3.0f),
                 QVector3D(0.0f, 0.0f, 0.0f),
                 QVector3D(0.0f, 1.0f, 0.0f));

    model.setToIdentity();

    QMatrix4x4 mvp = projection * view * model;
    m_program->setUniformValue("MVP", mvp);

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearColor(0, 0, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

    m_program->disableAttributeArray(0);
    m_program->disableAttributeArray(1);
    m_program->release();

    m_window->resetOpenGLState();
}
