#include "ModelLoader.h"
#include <iostream>

ModelLoader::ModelLoader()
{



}

ModelLoader::~ModelLoader()
{


}

/*load fbx file to model data structure*/
bool ModelLoader::Load(const std::string fileName, Model* m)
{
    Assimp::Importer fbxImport;

    const aiScene* loadedScene = fbxImport.ReadFile(fileName,aiProcessPreset_TargetRealtime_MaxQuality);

    if (!loadedScene)
    {
        std::cerr << "Unable to load file " << fileName << ": " << fbxImport.GetErrorString() << std::endl;
        return false;
    }

    /*reserve memory for meshes*/
    m->meshes.reserve(loadedScene->mNumMeshes);

    for (UINT i = 0; i < loadedScene->mNumMeshes; i++)
    {
        aiMesh* mesh = loadedScene->mMeshes[i];

        /*reserve memory for vertices*/
        m->meshes.push_back(new Mesh());
        m->meshes[i]->vertices.reserve(mesh->mNumVertices);

        /*get material from index*/
        aiMaterial* material = loadedScene->mMaterials[mesh->mMaterialIndex];
        aiColor4D ambient, diffuse, specular;
        float shine;

        aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient);
        aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
        aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular);
        aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shine);

        m->meshes[i]->material = Material::Standard();
        m->meshes[i]->material.Ambient = XMFLOAT4(ambient.r, ambient.g, ambient.b, ambient.a);
        m->meshes[i]->material.Diffuse = XMFLOAT4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
        m->meshes[i]->material.Specular = XMFLOAT4(specular.r, specular.g, specular.b, shine);


        /*get vertices: positions normals tex coords and tangentu */


        m->meshes[i]->hasTangentu = mesh->HasTangentsAndBitangents();
        m->meshes[i]->hasTextureCoordinates = mesh->HasTextureCoords(0);

        for (UINT v = 0; v < mesh->mNumVertices; v++)
        {

            aiVector3D tex(0);
            aiVector3D tangU(0);
            aiVector3D pos = mesh->mVertices[v];
            aiVector3D norm = mesh->mNormals[v];

            if (m->meshes[i]->hasTangentu)
            {
                tangU = mesh->mTangents[v];
            }
            if (m->meshes[i]->hasTextureCoordinates)
            {
                tex = mesh->mTextureCoords[0][v];
            }

            /*convert to vertex data format*/
            m->meshes[i]->vertices.push_back(Vertex::PosTexNormalTan(XMFLOAT3(pos.x, pos.z, pos.y), //!!! Z UP ??
                                            XMFLOAT2(tex.x , tex.y), //TODO tex coords
                                            XMFLOAT3(norm.x, norm.y, norm.z),
                                            XMFLOAT3(0.f, 0.f, 0.f) //TODO
            ));

        }

        /*get indices*/
        m->meshes[i]->indices.reserve((__int64)(mesh->mNumFaces) * 3);

        for (UINT j = 0; j < mesh->mNumFaces; j++)
        {
            m->meshes[i]->indices.push_back(mesh->mFaces[j].mIndices[0]);
            m->meshes[i]->indices.push_back(mesh->mFaces[j].mIndices[1]);
            m->meshes[i]->indices.push_back(mesh->mFaces[j].mIndices[2]);
        }


    }

    return true;
}