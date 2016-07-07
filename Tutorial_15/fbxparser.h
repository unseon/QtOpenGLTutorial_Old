#ifndef FBXPARSER_H
#define FBXPARSER_H

#include <QObject>
#include <fbxsdk.h>

class Scene;
class Node;
class Mesh;
class Material;

class FbxParser : public QObject
{
    Q_OBJECT
public:
    explicit FbxParser(QObject *parent = 0);


    Node* parseNode(FbxNode* node);
    Mesh* parseMesh(FbxMesh* mesh);
    Material* parseMaterial(FbxSurfaceMaterial *fMaterial);
    Scene* parseScene(FbxScene* scene);

    static FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);

signals:

public slots:



};

#endif // FBXPARSER_H
