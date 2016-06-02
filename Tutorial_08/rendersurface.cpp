#include "rendersurface.h"

#include <QQuickWindow>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLContext>
#include <QImage>
#include <qmath.h>
#include <QThread>
#include <QDir>

#include <fbxsdk.h>
#include <vector>

using std::vector;

RenderSurface::RenderSurface()
    : m_renderer(0),
      m_leftMouseDown(false),
      m_upKey(false),
      m_downKey(false),
      m_leftKey(false),
      m_rightKey(false)
{
    connect(this, &QQuickItem::windowChanged, this, &RenderSurface::handleWindowChanged);

    setAcceptedMouseButtons(Qt::AllButtons);

    m_loop = new QTimer(this);
    connect(m_loop, &QTimer::timeout, this, &RenderSurface::tick);
    m_loop->start(33);

    time.start();
}

void RenderSurface::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &RenderSurface::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &RenderSurface::cleanup, Qt::DirectConnection);

        win->setClearBeforeRendering(false);
    }
}

void RenderSurface::updateWindow()
{
    if (window())
        window()->update();
}

void RenderSurface::tick()
{
    static int counter = 0;
    int elapsedTime = time.nsecsElapsed();
    time.restart();

    //qDebug() << "handleIdle called - " << "counter:" << counter++ << "elapsed time: " << elapsedTime;

    if (m_leftKey) {
        m_renderer->setCamRotationY(m_renderer->camRotationY() - elapsedTime / 10000000);
        updateWindow();
    } else if (m_rightKey){
        m_renderer->setCamRotationY(m_renderer->camRotationY() + elapsedTime / 10000000);
        updateWindow();
    } else if (m_upKey) {
        m_renderer->setCamRotationX(m_renderer->camRotationX() - elapsedTime / 10000000);
        updateWindow();
    } else if (m_downKey) {
        m_renderer->setCamRotationX(m_renderer->camRotationX() + elapsedTime / 10000000);
        updateWindow();
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

void RenderSurface::sync()
{
    if (!m_renderer) {
        m_renderer = new Renderer();
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &Renderer::render, Qt::DirectConnection);
    }

    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setWindow(window());
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

        updateWindow();
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
    m_renderer->setCamDistance(curDist + event->angleDelta().y() * -0.005);

    updateWindow();
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
      m_texture(0),
      m_rotationX(45.0f),
      m_rotationY(45.0f),
      m_distance(10.0f)
{
    initializeOpenGLFunctions();

    updateCamera();

    initializeMesh();
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

void Renderer::initializeMesh()
{
//    static float vertices[] = {
//        -1.0f,-1.0f,-1.0f,
//        -1.0f,-1.0f, 1.0f,
//        -1.0f, 1.0f, 1.0f,

//         1.0f, 1.0f,-1.0f,
//        -1.0f,-1.0f,-1.0f,
//        -1.0f, 1.0f,-1.0f,

//         1.0f,-1.0f, 1.0f,
//        -1.0f,-1.0f,-1.0f,
//         1.0f,-1.0f,-1.0f,

//         1.0f, 1.0f,-1.0f,
//         1.0f,-1.0f,-1.0f,
//        -1.0f,-1.0f,-1.0f,

//        -1.0f,-1.0f,-1.0f,
//        -1.0f, 1.0f, 1.0f,
//        -1.0f, 1.0f,-1.0f,

//         1.0f,-1.0f, 1.0f,
//        -1.0f,-1.0f, 1.0f,
//        -1.0f,-1.0f,-1.0f,

//        -1.0f, 1.0f, 1.0f,
//        -1.0f,-1.0f, 1.0f,
//         1.0f,-1.0f, 1.0f,

//         1.0f, 1.0f, 1.0f,
//         1.0f,-1.0f,-1.0f,
//         1.0f, 1.0f,-1.0f,

//         1.0f,-1.0f,-1.0f,
//         1.0f, 1.0f, 1.0f,
//         1.0f,-1.0f, 1.0f,

//         1.0f, 1.0f, 1.0f,
//         1.0f, 1.0f,-1.0f,
//        -1.0f, 1.0f,-1.0f,

//         1.0f, 1.0f, 1.0f,
//        -1.0f, 1.0f,-1.0f,
//        -1.0f, 1.0f, 1.0f,

//         1.0f, 1.0f, 1.0f,
//        -1.0f, 1.0f, 1.0f,
//         1.0f,-1.0f, 1.0f
//    };

//    // Two UV coordinatesfor each vertex. They were created with Blender. You'll learn shortly how to do this yourself.
//     static float uvs[] = {
//        0.000059f, 1.0f-0.000004f,
//        0.000103f, 1.0f-0.336048f,
//        0.335973f, 1.0f-0.335903f,
//        1.000023f, 1.0f-0.000013f,
//        0.667979f, 1.0f-0.335851f,
//        0.999958f, 1.0f-0.336064f,
//        0.667979f, 1.0f-0.335851f,
//        0.336024f, 1.0f-0.671877f,
//        0.667969f, 1.0f-0.671889f,
//        1.000023f, 1.0f-0.000013f,
//        0.668104f, 1.0f-0.000013f,
//        0.667979f, 1.0f-0.335851f,
//        0.000059f, 1.0f-0.000004f,
//        0.335973f, 1.0f-0.335903f,
//        0.336098f, 1.0f-0.000071f,
//        0.667979f, 1.0f-0.335851f,
//        0.335973f, 1.0f-0.335903f,
//        0.336024f, 1.0f-0.671877f,
//        1.000004f, 1.0f-0.671847f,
//        0.999958f, 1.0f-0.336064f,
//        0.667979f, 1.0f-0.335851f,
//        0.668104f, 1.0f-0.000013f,
//        0.335973f, 1.0f-0.335903f,
//        0.667979f, 1.0f-0.335851f,
//        0.335973f, 1.0f-0.335903f,
//        0.668104f, 1.0f-0.000013f,
//        0.336098f, 1.0f-0.000071f,
//        0.000103f, 1.0f-0.336048f,
//        0.000004f, 1.0f-0.671870f,
//        0.336024f, 1.0f-0.671877f,
//        0.000103f, 1.0f-0.336048f,
//        0.336024f, 1.0f-0.671877f,
//        0.335973f, 1.0f-0.335903f,
//        0.667969f, 1.0f-0.671889f,
//        1.000004f, 1.0f-0.671847f,
//        0.667979f, 1.0f-0.335851f
//    };

//    m_vertexBufferData = vertices;
//    m_uvBufferData = uvs;

    FbxManager* mySdkManager = NULL;
    mySdkManager = FbxManager::Create();

    FbxIOSettings *ios = FbxIOSettings::Create(mySdkManager, IOSROOT);
    mySdkManager->SetIOSettings(ios);

    FbxImporter* myImporter = FbxImporter::Create(mySdkManager, "");

    const char* myImportFile = "../../cube.fbx";

    QFile file("../../cube.fbx");

    qDebug() << QDir::currentPath();
    qDebug() << "exist? : " << file.exists();

    if (!myImporter->Initialize(myImportFile, -1, mySdkManager->GetIOSettings())) {
        qDebug() << "Call to FbxImporter::Initialize() failed.";
        qDebug() << "Error returned: " << myImporter->GetStatus().GetErrorString();
    }


    FbxScene* lScene = FbxScene::Create(mySdkManager, "myScene");

    myImporter->Import(lScene);

    myImporter->Destroy();

    vector<float> vertexBuffer;
    vector<unsigned int> indexBuffer;

    FbxNode* lRootNode = lScene->GetRootNode();
    if(lRootNode) {
        for(int i = 0; i < lRootNode->GetChildCount(); i++) {
            FbxNode* pNode = lRootNode->GetChild(i);

            const char* nodeName = pNode->GetName();
            FbxDouble3 translation = pNode->LclTranslation.Get();
            FbxDouble3 rotation = pNode->LclRotation.Get();
            FbxDouble3 scaling = pNode->LclScaling.Get();

            qDebug() << "node name: " << nodeName;
            qDebug() << "translation: " << QString("%1, %2, %3").arg(translation[0]).arg(translation[1]).arg(translation[2]);
            qDebug() << "rotation: " << QString("%1, %2, %3").arg(rotation[0]).arg(rotation[1]).arg(rotation[2]);
            qDebug() << "translation: " << QString("%1, %2, %3").arg(scaling[0]).arg(scaling[1]).arg(scaling[2]);

            for(int i = 0; i < pNode->GetNodeAttributeCount(); i++) {
                FbxNodeAttribute* pAttribute = pNode->GetNodeAttributeByIndex(i);

                FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
                FbxString attrName = pAttribute->GetName();

                qDebug()<< "type: " << typeName.Buffer() << " name: " << attrName.Buffer();
            }

            FbxMesh* pMesh = pNode->GetMesh();



            if (pMesh) {
                int vertexCount = pMesh->GetControlPointsCount();
                qDebug() << "VertexCount: " << vertexCount;

                FbxVector4* lControlPoints = pMesh->GetControlPoints();

                for (int i = 0; i < vertexCount; i++) {
                    qDebug() << lControlPoints[i][0] << lControlPoints[i][1] << lControlPoints[i][2];
                }

                qDebug() << "PolygonCount: " << pMesh->GetPolygonCount();
                qDebug() << "PolygonVertexCount: " << pMesh->GetPolygonVertexCount();

                int polygonCount = pMesh->GetPolygonCount();
                int idx = 0;

                for (int i = 0; i < polygonCount; i++) {
                    int polygonSize = pMesh->GetPolygonSize(i);
                    qDebug() << "polygonSize: " << polygonSize;

                    if (polygonSize == 3) {
                        int* polygon = pMesh->GetPolygonVertices();
                        indexBuffer.push_back(polygon[idx]);
                        indexBuffer.push_back(polygon[idx + 1]);
                        indexBuffer.push_back(polygon[idx + 2]);
                    } else if (polygonSize == 4) {
                        int* polygon = pMesh->GetPolygonVertices();
                        indexBuffer.push_back(polygon[idx]);
                        indexBuffer.push_back(polygon[idx + 1]);
                        indexBuffer.push_back(polygon[idx + 2]);

                        indexBuffer.push_back(polygon[idx + 2]);
                        indexBuffer.push_back(polygon[idx + 3]);
                        indexBuffer.push_back(polygon[idx]);
                    }

                    idx += polygonSize;
                }

                for (int i = 0; i < pMesh->GetPolygonVertexCount(); i++) {
                    qDebug() << "PolygonVertex: " << pMesh->GetPolygonVertices()[i];
                }
            }



        }
    }




//    if( !scene)
//    {
//        qDebug() << "Error loading file: (assimp:) " << importer.GetErrorString();
//        return;
//    }

//    if(scene->HasMeshes())
//    {
//        // load only first mesh
//        aiMesh* mesh = scene->mMeshes[0];

//        m_sizeVertices = mesh->mNumVertices;
//        m_sizeUv = mesh->mNumVertices;
//        m_sizeIndices = mesh->mNumFaces;

//        qDebug() << "Mesh - vertices:" << m_sizeVertices << "uvs:" << m_sizeUv << "indices:" << m_sizeIndices;

//        m_vertexBufferData = new float[mesh->mNumVertices * 3];
//        m_uvBufferData = new float[mesh->mNumVertices * 2];
//        m_indexBufferData = new unsigned int[mesh->mNumFaces * 3];

//        for (int i = 0; i < mesh->mNumVertices; i++) {

//            aiVector3D &vec = mesh->mVertices[i];

//            m_vertexBufferData[i * 3] = vec.x;
//            m_vertexBufferData[i * 3 + 1] = vec.y;
//            m_vertexBufferData[i * 3 + 2] = vec.z;

//            //qDebug() << vec.x << vec.y << vec.z;

//            aiVector3D uv = mesh->mTextureCoords[0][i];

//            m_uvBufferData[i * 2] = uv.x;
//            m_uvBufferData[i * 2 + 1] = uv.y;

//            qDebug() << uv.x << uv.y;
//        }

//        for (int i = 0; i < mesh->mNumFaces; i++) {
//            aiFace face = mesh->mFaces[i];
//            m_indexBufferData[i * 3] = face.mIndices[0];
//            m_indexBufferData[i * 3 + 1] = face.mIndices[1];
//            m_indexBufferData[i * 3 + 2] = face.mIndices[2];

//            qDebug() << face.mIndices[0] << face.mIndices[1] << face.mIndices[2];
//        }


//    }
//    else
//    {
//        qDebug() << "Error: No meshes found";
//        return;
//    }
}

void Renderer::updateCamera()
{
    m_projection.setToIdentity();
    m_view.setToIdentity();
    m_model.setToIdentity();

    m_projection.perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    m_view.lookAt(QVector3D(0.0f, 0.0f, m_distance),
                 QVector3D(0.0f, 0.0f, 0.0f),
                 QVector3D(0.0f, 1.0f, 0.0f));

    m_view.rotate(m_rotationX, 1.0f, 0.0f, 0.0f);
    m_view.rotate(m_rotationY, 0.0f, 1.0f, 0.0f);
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


void Renderer::render()
{
    static GLuint elementBuffer;

    //qDebug() << "render called";

    updateCamera();

    if (!m_program) {
        m_program = new QOpenGLShaderProgram();
        m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/tutorial_05.vs");
        m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/tutorial_05.fs");

        m_program->bindAttributeLocation("vertices", 0);
        m_program->bindAttributeLocation("texCoord", 1);
        m_program->link();

        m_texture = new QOpenGLTexture(QImage(":/images/uvtemplate.png").mirrored());

        glGenBuffers(1, &elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_sizeIndices * 3 * sizeof(unsigned int), &m_indexBufferData[0], GL_STATIC_DRAW);
    }

    m_program->bind();

    m_program->setUniformValue("texture", 0);

    m_texture->bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(1);

    m_program->setAttributeArray(0, GL_FLOAT, m_vertexBufferData, 3);
    m_program->setAttributeArray(1, GL_FLOAT, m_uvBufferData, 2);

    QMatrix4x4 mvp = m_projection * m_view * m_model;
    m_program->setUniformValue("MVP", mvp);

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);

    glClearColor(0, 0, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

    glDrawElements(GL_TRIANGLES, m_sizeIndices * 3, GL_UNSIGNED_INT, (void*)0);

    m_program->disableAttributeArray(0);
    m_program->disableAttributeArray(1);
    m_program->release();

    m_window->resetOpenGLState();
}
