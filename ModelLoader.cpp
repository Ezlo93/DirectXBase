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