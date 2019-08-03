#include "ModelLoader.h"
#include <iostream>
#include <fstream>

#pragma warning( disable : 26451)

#define CHECK_NORMALS

bool ModelLoader::LoadB3D(const std::string& fileName, Model* m)
{
    /*open file*/
    streampos fileSize;
    ifstream file(fileName, std::ios::binary);

    /*get file size*/
    file.seekg(0, ios::end);
    fileSize = file.tellg();
    file.seekg(0, ios::beg);

    /*check header*/
    bool header = true;
    char headerBuffer[4] = { 'b','3','d','f' };

    for (int i = 0; i < 4; i++)
    {
        char temp;
        file.read(&temp, sizeof(temp));

        if (temp != headerBuffer[i])
        {
            header = false;
            break;
        }
    }

    if (header == false)
    {
        DBOUT("b3d header incorrect");
        return false;
    }

    /*number of meshes*/
    char numMeshes = 0;
    file.read(&numMeshes, sizeof(numMeshes));

    m->meshes.reserve((size_t)numMeshes);


    for (char i = 0; i < numMeshes; i++)
    {
        m->meshes.push_back(new Mesh());

        /*read material*/

        file.read((char*)(&m->meshes[i]->material.Ambient.x), sizeof(float));
        file.read((char*)(&m->meshes[i]->material.Ambient.y), sizeof(float));
        file.read((char*)(&m->meshes[i]->material.Ambient.z), sizeof(float));
        m->meshes[i]->material.Ambient.w = 0.f;

        file.read((char*)(&m->meshes[i]->material.Diffuse.x), sizeof(float));
        file.read((char*)(&m->meshes[i]->material.Diffuse.y), sizeof(float));
        file.read((char*)(&m->meshes[i]->material.Diffuse.z), sizeof(float));
        m->meshes[i]->material.Diffuse.w = 0.f;

        file.read((char*)(&m->meshes[i]->material.Specular.x), sizeof(float));
        file.read((char*)(&m->meshes[i]->material.Specular.y), sizeof(float));
        file.read((char*)(&m->meshes[i]->material.Specular.z), sizeof(float));

        file.read((char*)(&m->meshes[i]->material.Specular.w), sizeof(float));

        /*read map strings*/

        short slen = 0;
        file.read((char*)(&slen), sizeof(short));

        char* dmap = new char[ (int)(slen) + 1];
        file.read(dmap, slen);
        dmap[slen] = '\0';

        slen = 0;
        file.read((char*)(&slen), sizeof(short));

        char* nmap = new char[(int)(slen)+ 1];
        file.read(nmap, slen);
        nmap[slen] = '\0';

        slen = 0;
        file.read((char*)(&slen), sizeof(short));

        char* bmap = new char[(int)(slen)+ 1];
        file.read(bmap, slen);
        bmap[slen] = '\0';

        m->meshes[i]->diffuseMapID = dmap;
        m->meshes[i]->normalMapID = nmap;
        m->meshes[i]->bumpMapID = bmap;

        delete[] dmap; delete[] nmap; delete[] bmap;

        /*read number of vertices*/
        int vertCount = 0;
        file.read((char*)(&vertCount), sizeof(vertCount));

        m->meshes[i]->vertices.resize(vertCount);

        /*vertex properties*/
        for (int j = 0; j < vertCount; j++)
        {
            file.read((char*)(&m->meshes[i]->vertices[j].Pos.x), sizeof(float));
            file.read((char*)(&m->meshes[i]->vertices[j].Pos.y), sizeof(float));
            file.read((char*)(&m->meshes[i]->vertices[j].Pos.z), sizeof(float));

            file.read((char*)(&m->meshes[i]->vertices[j].Tex.x), sizeof(float));
            file.read((char*)(&m->meshes[i]->vertices[j].Tex.y), sizeof(float));

            file.read((char*)(&m->meshes[i]->vertices[j].Normal.x), sizeof(float));
            file.read((char*)(&m->meshes[i]->vertices[j].Normal.y), sizeof(float));
            file.read((char*)(&m->meshes[i]->vertices[j].Normal.z), sizeof(float));

#ifdef CHECK_NORMALS
            if (m->meshes[i]->vertices[j].Normal.x > 1 ||
                m->meshes[i]->vertices[j].Normal.y > 1 ||
                m->meshes[i]->vertices[j].Normal.z > 1)
            {
                throw std::exception("normal > 1");
            }
#endif

            file.read((char*)(&m->meshes[i]->vertices[j].TangentU.x), sizeof(float));
            file.read((char*)(&m->meshes[i]->vertices[j].TangentU.y), sizeof(float));
            file.read((char*)(&m->meshes[i]->vertices[j].TangentU.z), sizeof(float));
        }

        /*number of indices*/

        int vInd = 0;
        file.read((char*)(&vInd), sizeof(vInd));
        m->meshes[i]->indices.resize(vInd);

        for (int j = 0; j < vInd; j++)
        {
            file.read((char*)(&m->meshes[i]->indices[j]), sizeof(int));
        }

    }


    return true;
}