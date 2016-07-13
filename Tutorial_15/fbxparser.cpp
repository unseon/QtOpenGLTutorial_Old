#include "fbxparser.h"
#include <fbxsdk.h>

#include "scene.h"
#include "node.h"
#include "mesh.h"
#include "directionallight.h"

FbxParser::FbxParser(QObject *parent) : QObject(parent)
{

}

FbxString FbxParser::GetAttributeTypeName(FbxNodeAttribute::EType type) {
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

Scene* FbxParser::parseScene(FbxScene* fScene)
{
    m_scene = new Scene();

    FbxNode* fRootNode = fScene->GetRootNode();

    if (fRootNode) {
        m_scene->m_root = parseNode(fRootNode);
    }

    return m_scene;
}

Node* FbxParser::parseNode(FbxNode* fNode)
{
    Node* node = new Node();
    // basic info
    const char* nodeName = fNode->GetName();

    node->m_name = fNode->GetName();

    // transform info
    FbxDouble3 translation = fNode->LclTranslation.Get();
    FbxDouble3 rotation = fNode->LclRotation.Get();
    FbxDouble3 scaling = fNode->LclScaling.Get();
    FbxDouble3 postRotation = fNode->PostRotation.Get();

    node->m_position = QVector3D(translation[0], translation[1], translation[2]);
    node->m_rotation = QVector3D(rotation[0], rotation[1], rotation[2]);
    node->m_rotation += QVector3D(postRotation[0], postRotation[1], postRotation[2]);
    node->m_scale = QVector3D(scaling[0], scaling[1], scaling[2]);

    qDebug() << "m_name" << node->m_name;
    qDebug() << "m_position" << node->m_position;
    qDebug() << "m_rotation" << node->m_rotation;
    qDebug() << "m_scale" << node->m_scale;
    //qDebug() << "pivot_rotation" << pivotRotation[0] << pivotRotation[1] << pivotRotation[2];

    for(int i = 0; i < fNode->GetNodeAttributeCount(); i++) {
        FbxNodeAttribute* pAttribute = fNode->GetNodeAttributeByIndex(i);

        FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
        FbxString attrName = pAttribute->GetName();

        qDebug()<< "type: " << typeName.Buffer() << " name: " << attrName.Buffer();
    }

    if (fNode->GetMesh()) {
        node->m_mesh = parseMesh(fNode->GetMesh());
        node->m_mesh->m_node = node;
    } else if (fNode->GetLight()) {
        qDebug() << "Directional Light";
        node->m_light = parseLight(fNode->GetLight());

        if (m_scene->m_mainLight == NULL) {
            m_scene->m_mainLight = node;
        }
    }

    for (int i = 0; i < fNode->GetChildCount(); i++) {
        FbxNode* fChildNode = fNode->GetChild(i);
        Node* childNode = parseNode(fChildNode);
        node->addChild(childNode);
    }

    return node;
}

DirectionalLight* FbxParser::parseLight(FbxLight *fLight)
{
    DirectionalLight* light = new DirectionalLight();

    int lightType = fLight->LightType.Get();
    FbxDouble3 c = fLight->Color.Get();
    light->m_color = QVector4D(c[0], c[1], c[2], 1.0f);

    return light;
}

Mesh* FbxParser::parseMesh(FbxMesh* fMesh)
{
    Mesh* mesh = new Mesh();

    // build vertex, uv buffers
    qDebug() << "PolygonCount: " << fMesh->GetPolygonCount();
    int polygonCount = fMesh->GetPolygonCount();
    FbxGeometryElementUV* leUV = fMesh->GetElementUV(0);
    FbxGeometryElementNormal* leNormal = fMesh->GetElementNormal(0);
    FbxGeometryElementTangent* leTangent = fMesh->GetElementTangent(0);
    FbxGeometryElementBinormal* leBiTangent = fMesh->GetElementBinormal(0);

    int vertexId = 0;

    for (int i = 0; i < polygonCount; i++) {

        int polygonSize = fMesh->GetPolygonSize(i);

        for (int j = 0; j < polygonSize; j++) {
            int lControlPointIndex = fMesh->GetPolygonVertex(i, j);
            FbxVector4 vert = fMesh->GetControlPoints()[lControlPointIndex];

            //qDebug() << "vertex: " << vert[0] << vert[1] << vert[2];
            mesh->m_vertices.push_back(vert[0]);
            mesh->m_vertices.push_back(vert[1]);
            mesh->m_vertices.push_back(vert[2]);

            int lTextureUVIndex = fMesh->GetTextureUVIndex(i, j);

            if (leUV) {
                FbxVector2 uv = leUV->GetDirectArray().GetAt(lTextureUVIndex);

                //qDebug() << "uv: " << uv[0] << uv[1];
                mesh->m_uvs.push_back(uv[0]);
                mesh->m_uvs.push_back(uv[1]);
            }

            if (leNormal) {
                FbxVector4 normal = leNormal->GetDirectArray().GetAt(vertexId);
                //qDebug() << "idx: " << lControlPointIndex <<"normal: " << normal[0] << normal[1] << normal[2];
                mesh->m_normals.push_back(normal[0]);
                mesh->m_normals.push_back(normal[1]);
                mesh->m_normals.push_back(normal[2]);
            }

            if (leTangent) {
                FbxVector4 tangent = leTangent->GetDirectArray().GetAt(vertexId);
                //qDebug() << "idx: " << lControlPointIndex <<"tangent: " << tangent[0] << tangent[1] << tangent[2];
                mesh->m_tangents.push_back(tangent[0]);
                mesh->m_tangents.push_back(tangent[1]);
                mesh->m_tangents.push_back(tangent[2]);
            }

            if (leBiTangent) {
                FbxVector4 bitangent = leBiTangent->GetDirectArray().GetAt(vertexId);
                //qDebug() << "idx: " << lControlPointIndex <<"bitangent: " << bitangent[0] << bitangent[1] << bitangent[2];
                mesh->m_bitangents.push_back(bitangent[0]);
                mesh->m_bitangents.push_back(bitangent[1]);
                mesh->m_bitangents.push_back(bitangent[2]);
            }

            vertexId++;
        }
    }

    // build index buffer
    int idx = 0;

    for (int i = 0; i < polygonCount; i++) {

        int polygonSize = fMesh->GetPolygonSize(i);

        //qDebug() << "polygonSize: " << polygonSize;

        if (polygonSize == 3) {
            mesh->m_indices.push_back(idx);
            mesh->m_indices.push_back(idx + 1);
            mesh->m_indices.push_back(idx + 2);
        } else if (polygonSize == 4) {
            mesh->m_indices.push_back(idx);
            mesh->m_indices.push_back(idx + 1);
            mesh->m_indices.push_back(idx + 2);

            mesh->m_indices.push_back(idx + 2);
            mesh->m_indices.push_back(idx + 3);
            mesh->m_indices.push_back(idx);
        }

        idx += polygonSize;
    }

    // build texture info
    FbxSurfaceMaterial *fMaterial = fMesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(0);

    if (fMaterial != NULL) {
        mesh->m_material = parseMaterial(fMaterial);
    }

    return mesh;
}

Material* FbxParser::parseMaterial(FbxSurfaceMaterial *fMaterial)
{
    Material* material = new Material;

    qDebug() << FbxLayerElement::sTextureChannelNames[0];
    FbxProperty lProperty = fMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[0]);
    FbxTexture* lTexture = lProperty.GetSrcObject<FbxTexture>(0);
    FbxFileTexture *lFileTexture = FbxCast<FbxFileTexture>(lTexture);

    if (lFileTexture != NULL) {
        qDebug() << "texture file: " << (char *) lFileTexture->GetFileName();
        material->m_textureFile = (char *) lFileTexture->GetFileName();
    }

    qDebug() << FbxLayerElement::sTextureChannelNames[9];
    FbxProperty lProperty2 = fMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[9]);
    FbxTexture* lTexture2 = lProperty2.GetSrcObject<FbxTexture>(0);
    FbxFileTexture *lFileTexture2 = FbxCast<FbxFileTexture>(lTexture2);

    if (lFileTexture2 != NULL) {
        qDebug() << "normalmap file: " << (char *) lFileTexture2->GetFileName();
        material->m_normalmapFile = (char *) lFileTexture2->GetFileName();
    }

    FbxPropertyT<FbxDouble3> lKFbxDouble3;
    FbxPropertyT<FbxDouble> lKFbxDouble1;

    lKFbxDouble3 =((FbxSurfacePhong *) fMaterial)->Ambient;
    material->m_ambient = QVector4D(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2], 1.0f);

    lKFbxDouble3 =((FbxSurfacePhong *) fMaterial)->Diffuse;
    material->m_diffuse = QVector4D(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2], 1.0f);

    lKFbxDouble3 =((FbxSurfacePhong *) fMaterial)->Specular;
    material->m_specular = QVector4D(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2], 1.0f);

    lKFbxDouble3 =((FbxSurfacePhong *) fMaterial)->Emissive;
    material->m_emission = QVector4D(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2], 1.0f);

//    lKFbxDouble1 =((FbxSurfacePhong *) fMaterial)->TransparencyFactor;
//    material->m_opacity = lKFbxDouble1.Get();

    lKFbxDouble1 =((FbxSurfacePhong *) fMaterial)->Shininess;
    material->m_shininess = lKFbxDouble1.Get();

    //lKFbxDouble1 =((FbxSurfacePhong *) lMaterial)->ReflectionFactor;

    return material;
}
