#include "ModelLoader.h"
#include <iostream>
#include <fstream>

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

    DBOUT("Loading model " << fileName.c_str() << endl);

    const aiScene* loadedScene = fbxImport.ReadFile(fileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
                                                    
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

        DBOUT("Mesh " << i << " (" << mesh->mName.C_Str() << ") with " << mesh->mNumVertices << " vertices" << endl;);

        /*get material from index*/
        aiMaterial* material = loadedScene->mMaterials[mesh->mMaterialIndex];

        //aiColor4D ambient, diffuse, specular;
        aiColor4D ambient, diffuse, specular;
        float shine;

        aiString name;
        material->Get(AI_MATKEY_NAME, name);

        DBOUT("Material: " << name.data << endl);


        aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient);
        aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
        aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular);
        aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shine);

        /*
        m->meshes[i]->material = Material::Standard();
        m->meshes[i]->material.Ambient = XMFLOAT4(ambient.r, ambient.g, ambient.b, 0.f);
        m->meshes[i]->material.Diffuse = XMFLOAT4(diffuse.r, diffuse.g, diffuse.b, 0.f);
        m->meshes[i]->material.Specular = XMFLOAT4(specular.r, specular.g, specular.b, shine);
        */

        m->meshes[i]->material.Ambient = XMFLOAT4(1.f, 1.f, 1.f, 0.f);
        m->meshes[i]->material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 0.f);
        m->meshes[i]->material.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, shine);

        DBOUT("Ambient: " << m->meshes[i]->material.Ambient.x << " " << m->meshes[i]->material.Ambient.y << " " << m->meshes[i]->material.Ambient.z << endl);
        DBOUT("Diffuse: " << m->meshes[i]->material.Diffuse.x << " " << m->meshes[i]->material.Diffuse.y << " " << m->meshes[i]->material.Diffuse.z << endl);
        DBOUT("Specular: " << m->meshes[i]->material.Specular.x << " " << m->meshes[i]->material.Specular.y << " " << m->meshes[i]->material.Specular.z << endl);

        m->meshes[i]->diffuseMapID = "none";
        m->meshes[i]->normalMapID = "none";
        m->meshes[i]->bumpMapID = "none";

        if ( material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString Path;

            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
            {
                char id[1024];
                _splitpath_s(Path.data, NULL, 0, NULL, 0, id, 1024, NULL, 0);
                m->meshes[i]->diffuseMapID = id;
                DBOUT("Diffuse Map: " << id << endl);
            }
        }

        if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            aiString Path;

            if (material->GetTexture(aiTextureType_NORMALS, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
            {
                char id[1024];
                _splitpath_s(Path.data, NULL, 0, NULL, 0, id, 1024, NULL, 0);
                m->meshes[i]->normalMapID = id;
                DBOUT("Normal Map: " << id << endl);
            }

        }

        if (material->GetTextureCount(aiTextureType_DISPLACEMENT) > 0)
        {
            aiString Path;

            if (material->GetTexture(aiTextureType_DISPLACEMENT, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
            {
                char id[1024];
                _splitpath_s(Path.data, NULL, 0, NULL, 0, id, 1024, NULL, 0);
                m->meshes[i]->bumpMapID = id;
                DBOUT("Bump Map: " << id << endl);
            }

        }

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
            m->meshes[i]->vertices.push_back(Vertex::PosTexNormalTan(XMFLOAT3(pos.x, pos.y, pos.z),
                                            XMFLOAT2(tex.x , tex.y),
                                            XMFLOAT3(norm.x, norm.y, norm.z),
                                            XMFLOAT3(tangU.x , tangU.y, tangU.z)
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

bool ModelLoader::LoadBas(const std::string fileName, Model* m)
{
    std::ifstream fin(fileName);

    if (!fin)
    {
        return false;
    }

    UINT vcount = 0;
    UINT tcount = 0;
    std::string ignore;

    fin >> ignore >> vcount;
    fin >> ignore >> tcount;
    fin >> ignore >> ignore >> ignore >> ignore;

    m->meshes.push_back(new Mesh());
    m->meshes[0]->vertices.resize(vcount);

    for (UINT i = 0; i < vcount; ++i)
    {
        fin >> m->meshes[0]->vertices[i].Pos.x >> m->meshes[0]->vertices[i].Pos.y >> m->meshes[0]->vertices[i].Pos.z;
        fin >> m->meshes[0]->vertices[i].Normal.x >> m->meshes[0]->vertices[i].Normal.y >> m->meshes[0]->vertices[i].Normal.z;
    }

    fin >> ignore;
    fin >> ignore;
    fin >> ignore;

    int indexCount = 3 * tcount;
    m->meshes[0]->indices.resize(indexCount);

    for (int i = 0; i < tcount; ++i)
    {
        fin >> m->meshes[0]->indices[i * 3 + 0] >> m->meshes[0]->indices[i * 3 + 1] >> m->meshes[0]->indices[i * 3 + 2];
    }

    Material::Standard mat;
    mat.Ambient = XMFLOAT4(0.1f, 0.2f, 0.3f, 1.0f);
    mat.Diffuse = XMFLOAT4(0.2f, 0.4f, 0.6f, 1.0f);
    mat.Specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);

    m->meshes[0]->material = mat;
 
   fin.close();

   return true;
}
