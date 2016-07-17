#include "rendersurface.h"

#include <QQuickWindow>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLContext>
#include <QImage>
#include <qmath.h>
#include <QThread>
#include <QDir>
#include <QPainter>

#include <fbxsdk.h>
#include <vector>
#include <cmath>

#include "scene.h"
#include "node.h"
#include "directionallight.h"
#include "fbxparser.h"
#include "material.h"

using std::vector;
using std::floor;

RenderSurface::RenderSurface(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      m_renderer(0),
      m_leftMouseDown(false),
      m_upKey(false),
      m_downKey(false),
      m_leftKey(false),
      m_rightKey(false),
      m_fps(0.0f)
{
    //connect(this, &QQuickItem::windowChanged, this, &RenderSurface::handleWindowChanged);

    setAcceptedMouseButtons(Qt::AllButtons);

    m_loop = new QTimer(this);
    connect(m_loop, &QTimer::timeout, this, &RenderSurface::tick);
    //m_loop->start(33);

    time.start();
}

void RenderSurface::paint(QPainter *painter)
{
    //qDebug() << "paint called";

    if (!m_renderer) {
        m_renderer = new Renderer();
        //m_renderer->setWindow(window());
    }


    m_renderer->setViewportSize(QSize(int(width()), int(height())));
    m_renderer->render();

    //painter->fillRect(0, 0, width(), height(), Qt::red);
    painter->drawImage(0, 0, m_renderer->m_surface);
}

void RenderSurface::tick()
{
    static int counter = 0;
    int elapsedTime = time.nsecsElapsed();
    time.restart();

    //qDebug() << elapsedTime / 1000000;

    m_fps = floor(1.0f / elapsedTime * 1000000000);
    emit fpsChanged();

    //qDebug() << "handleIdle called - " << "counter:" << counter++ << "elapsed time: " << elapsedTime;

    if (m_leftKey) {
        m_renderer->setCamRotationY(m_renderer->camRotationY() - elapsedTime / 10000000);
        update();
    } else if (m_rightKey){
        m_renderer->setCamRotationY(m_renderer->camRotationY() + elapsedTime / 10000000);
        update();
    } else if (m_upKey) {
        m_renderer->setCamRotationX(m_renderer->camRotationX() - elapsedTime / 10000000);
        update();
    } else if (m_downKey) {
        m_renderer->setCamRotationX(m_renderer->camRotationX() + elapsedTime / 10000000);
        update();
    }

    //QThread::usleep(16666);
}

void RenderSurface::cleanup()
{
    if (m_renderer) {
        delete m_renderer;
        m_renderer = 0;
    }
}

void RenderSurface::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "Mouse Move";

    if (m_leftMouseDown == true) {
        QPointF currentPos = event->localPos();
        QPointF diff = currentPos - m_originPos;

        float rotX = m_originRotationX + diff.y() * 0.5;
        float rotY = m_originRotationY + diff.x() * 0.5;

        m_renderer->setCamRotationX(rotX);
        m_renderer->setCamRotationY(rotY);

        update();
    }
}

void RenderSurface::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_leftMouseDown = true;
        m_originPos = event->localPos();
        m_originRotationX = m_renderer->camRotationX();
        m_originRotationY = m_renderer->camRotationY();
    }
}

void RenderSurface::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "Mouse Release";
    if (event->button() == Qt::LeftButton) {
        m_leftMouseDown = false;
    }
}

void RenderSurface::wheelEvent(QWheelEvent *event)
{
    qDebug() << "Wheel Event" << event->angleDelta();
    float curDist = m_renderer->camDistance();
    m_renderer->setCamDistance(curDist + event->angleDelta().y() * -0.02);

    update();
}

void RenderSurface::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Left:
        m_leftKey = true;
        break;
    case Qt::Key_Right:
        m_rightKey = true;
        break;
    case Qt::Key_Up:
        m_upKey = true;
        break;
    case Qt::Key_Down:
        m_downKey = true;
        break;
    default:
        break;
    }
}

void RenderSurface::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Left:
        m_leftKey = false;
        break;
    case Qt::Key_Right:
        m_rightKey = false;
        break;
    case Qt::Key_Up:
        m_upKey = false;
        break;
    case Qt::Key_Down:
        m_downKey = false;
        break;
    default:
        break;
    }
}

Renderer::Renderer()
    : m_program(0),
      m_surfaceProgram(0),
      m_texture(0),
//      m_rotationX(45.0f),
//      m_rotationY(45.0f),
      m_rotationX(0.0f),
      m_rotationY(0.0f),
      m_distance(10.0f),
      m_surface(300, 200, QImage::QImage::Format_ARGB32),
      m_data(0),
    　m_frameBuffer(0),
    m_renderedTexture(0),
    m_depthRenderBuffer(0),
    m_scene(0)
{
    initializeOpenGLFunctions();

    initializeScene();

    initializeSurface();
}

Renderer::~Renderer()
{
}

FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
    switch(type) {
        case FbxNodeAttribute::eUnknown: return "unidentified";
        case FbxNodeAttribute::eNull: return "null";
        case FbxNodeAttribute::eMarker: return "marker";
        case FbxNodeAttribute::eSkeleton: return "skeleton";
        case FbxNodeAttribute::eMesh: return "mesh";
        case FbxNodeAttribute::eNurbs: return "nurbs";
        case FbxNodeAttribute::ePatch: return "patch";
        case FbxNodeAttribute::eCamera: return "camera";
        case FbxNodeAttribute::eCameraStereo: return "stereo";
        case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
        case FbxNodeAttribute::eLight: return "light";
        case FbxNodeAttribute::eOpticalReference: return "optical reference";
        case FbxNodeAttribute::eOpticalMarker: return "marker";
        case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
        case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
        case FbxNodeAttribute::eBoundary: return "boundary";
        case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
        case FbxNodeAttribute::eShape: return "shape";
        case FbxNodeAttribute::eLODGroup: return "lodgroup";
        case FbxNodeAttribute::eSubDiv: return "subdiv";
        default: return "unknown";
    }
}

void Renderer::initializeScene()
{
    FbxManager* mySdkManager = NULL;
    mySdkManager = FbxManager::Create();

    FbxIOSettings *ios = FbxIOSettings::Create(mySdkManager, IOSROOT);
    mySdkManager->SetIOSettings(ios);

    FbxImporter* myImporter = FbxImporter::Create(mySdkManager, "");

    //const char* myImportFile = "../../plane.fbx";
    const char* myImportFile = "../../objects.fbx";
    //const char* myImportFile = "../../objects_01.fbx";
    //const char* myImportFile = "../../monkey.fbx";
    //const char* myImportFile = "../../cube.fbx";
    //const char* myImportFile = "../../solid_cube.fbx";

    QFile file(myImportFile);

    qDebug() << QDir::currentPath();
    qDebug() << "exist? : " << file.exists();

    if (!myImporter->Initialize(myImportFile, -1, mySdkManager->GetIOSettings())) {
        qDebug() << "Call to FbxImporter::Initialize() failed.";
        qDebug() << "Error returned: " << myImporter->GetStatus().GetErrorString();
    }


    FbxScene* lScene = FbxScene::Create(mySdkManager, "myScene");

    myImporter->Import(lScene);

    myImporter->Destroy();

    FbxParser fbxParser;

    m_scene = fbxParser.parseScene(lScene);

    updateCamera();
}

void Renderer::initializeSurface()
{

}

void Renderer::updateCamera()
{
    m_scene->m_viewMatrix.setToIdentity();
    m_scene->m_modelMatrix.setToIdentity();

    m_scene->m_viewMatrix.lookAt(QVector3D(0.0f, 0.0f, m_distance),
                 QVector3D(0.0f, 0.0f, 0.0f),
                 QVector3D(0.0f, 1.0f, 0.0f));

    m_scene->m_modelMatrix.rotate(m_rotationX, 1.0f, 0.0f, 0.0f);
    m_scene->m_modelMatrix.rotate(m_rotationY, 0.0f, 1.0f, 0.0f);
}

float Renderer::camRotationX()
{
    return m_rotationX;
}

float Renderer::camRotationY()
{
    return m_rotationY;
}

float Renderer::camDistance()
{
    return m_distance;
}

void Renderer::setCamRotationX(float value)
{
    m_rotationX = value;
}

void Renderer::setCamRotationY(float value)
{
    m_rotationY = value;
}

void Renderer::setCamDistance(float value)
{
    m_distance = value;
}


void Renderer::setViewportSize(const QSize &size)
{
    m_viewportSize = size;
    m_isViewportDirty = true;
}

void Renderer::prepareRender()
{
    updateCamera();

    if (m_frameBuffer == 0 ) {
        qDebug() << "glBufferData: " << m_indices.size() * 3;
        glGenBuffers(1, &m_elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);


        // frame buffer initialize
        glGenFramebuffers(1, &m_frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);


        glGenTextures(1, &m_renderedTexture);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, m_renderedTexture);

        // Give an empty image to OpenGL ( the last "0" )
        //glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, 300, 200, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

        // Poor filtering. Needed !
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderedTexture, 0);
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers);


        glGenRenderbuffers(1, &m_depthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
        //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 300, 200);
        //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer);
    }

    m_scene->m_mainLight->m_light->prepare();

    if (m_isViewportDirty) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_renderedTexture);
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, m_viewportSize.width(), m_viewportSize.height(), 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, m_scene->m_mainLight->m_light->m_shadowMapTexture);
        //glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, m_viewportSize.width(), m_viewportSize.height(), 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, m_viewportSize.width(), m_viewportSize.height(), 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_viewportSize.width(), m_viewportSize.height());

        if (m_data) {
            delete[] m_data;
        }

        m_data = new uchar[m_viewportSize.width() * m_viewportSize.height() * 4];

        m_isViewportDirty = false;
    }
}

void Renderer::render()
{
    //qDebug() << "render called";

    prepareRender();



    if (m_scene->m_mainLight) {
        m_scene->m_mainLight->m_light->prepare();

        QVector4D forward(0, 0, -1, 0);

        QVector3D up(0, 1, 0);

        QMatrix4x4 localToWorld = m_scene->m_modelMatrix * m_scene->m_mainLight->netMatrix();
        QMatrix4x4 normalWorld = localToWorld.inverted().transposed();

        QVector3D viewPoint = localToWorld * QVector3D(0, 0, 0);
        //QVector4D viewForward = QVector4D((normalWorld * forward).toVector3D()).normalized();
        QVector4D viewForward = normalWorld * forward;
        //QVector3D viewTarget = (localToWorld * forwardPosition).toVector3D();
        //QVector3D viewTarget(0, 0, 0);
        QVector3D viewTarget = viewPoint + viewForward.toVector3D();
        QVector3D viewUp = normalWorld * up;

        QMatrix4x4 lightViewMatrix;

        lightViewMatrix.setToIdentity();
        lightViewMatrix.lookAt(viewPoint,
                               viewTarget,
                               viewUp.normalized());

        m_scene->m_viewMatrix = lightViewMatrix;

        m_scene->m_projectionMatrix.setToIdentity();
        m_scene->m_projectionMatrix.ortho(-20, 20, -20, 20, -60, 60);

    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_scene->m_mainLight->m_light->m_shadowMapFrameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer);

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height()); // Render on the whole framebuffer, complete from the lower left corner to the

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);

    glClearColor(0, 0, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_scene) {
        Material shadowMapMaterial("shadow_map_shading");
        m_scene->render(&shadowMapMaterial);
    }

    // bitblt to m_surface
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_scene->m_mainLight->m_light->m_shadowMapTexture);

    //float* depthData = new float[m_viewportSize.width() * m_viewportSize.height()];

    glReadPixels(0, 0, m_viewportSize.width(), m_viewportSize.height(), GL_RGBA, GL_UNSIGNED_BYTE, m_data);


    m_surface = QImage(m_data, m_viewportSize.width(), m_viewportSize.height(), QImage::Format_RGBA8888).mirrored();

    updateCamera();
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer);

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height()); // Render on the whole framebuffer, complete from the lower left corner to the

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (m_scene) {
        m_scene->m_projectionMatrix.setToIdentity();
        m_scene->m_projectionMatrix.perspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
        m_scene->render();
    }


    // bitblt to m_surface
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_renderedTexture);
    glReadPixels(0, 0, m_viewportSize.width(), m_viewportSize.height(), GL_RGBA, GL_UNSIGNED_BYTE, m_data);

    m_surface = QImage(m_data, m_viewportSize.width(), m_viewportSize.height(), QImage::Format_RGBA8888).mirrored();
}
