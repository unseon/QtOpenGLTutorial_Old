#ifndef MESH_H
#define MESH_H

#include "node.h"
#include "material.h"
#include <vector>

using std::vector;

class Material;

class Mesh
{
public:
    Mesh(Node* node);

    Node* m_node;

    vector<float> m_vertices;
    vector<float> m_uvs;
    vector<float> m_normals;
    vector<float> m_tangents;
    vector<float> m_bitangents;

    vector<unsigned int> m_indices;

    Material* m_material;

    void draw(Scene* scene);
};

#endif // MESH_H
