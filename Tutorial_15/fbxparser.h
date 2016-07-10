#ifndef FBXPARSER_H
#define FBXPARSER_H

#include <QObject>
#include <fbxsdk.h>

class Scene;
class Node;
class Mesh;
class Material;
class DirectionalLight;

class FbxParser : public QObject
{
    Q_OBJECT
public:
    explicit FbxParser(QObject *parent = 0);


    Node* parseNode(FbxNode* node);
    Mesh* parseMesh(FbxMesh* mesh);
    Material* parseMaterial(FbxSurfaceMaterial *fMaterial);
    DirectionalLight* parseLight(FbxLight* fLght);
    Scene* parseScene(FbxScene* fScene);

    static FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);

private:
    Scene* m_scene;

signals:

public slots:



};

#endif // FBXPARSER_H
