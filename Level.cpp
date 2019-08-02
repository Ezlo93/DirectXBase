#include "Level.h"


Level::Level(ResourceManager* r)
{
    res = r;
}

Level::~Level()
{
    /*clear pointer maps*/


}

bool Level::LoadLevel(std::string fileName)
{
    std::string buf;
    /*open file and check*/
    buf = LEVEL_PATH;
    buf.append(fileName);

    std::ifstream fin(buf);

    buf.clear();

    if (!fin.is_open())
    {
        throw std::exception("failed to load level");
        return false;
    }

    /*load static models*/
    fin >> buf;

    ASSERT(buf.compare("[static]") == 0);
    ReadStaticModels(fin);


    fin.close();

    return true;
}

void Level::ReadStaticModels(std::ifstream& fin)
{
    int objCount, objID;
    std::string mID, diff, norm, effectID;

    fin >> objCount;

    ASSERT(objCount > 0);


    for (int i = 0; i < objCount; i++)
    {
        mID.clear(); effectID.clear();
        fin >> objID;
        fin >> mID; //modelID

        ModelInstanceStatic* mis = new ModelInstanceStatic(res, mID);

        fin >> effectID;

        if (effectID == "basictexture")
        {
            mis->usedShader = UShader::Basic;
            mis->usedTechnique = UTech::Basic;
        }
        else if (effectID == "basicnormal")
        {
            mis->usedShader = UShader::Basic;
            mis->usedTechnique = UTech::BasicNormalMap;
        }
        else if (effectID == "basicnotexture")
        {
            mis->usedShader = UShader::Basic;
            mis->usedTechnique = UTech::BasicNoTexture;
        }
        else if (effectID == "basicnolighting")
        {
            mis->usedShader = UShader::Basic;
            mis->usedTechnique = UTech::BasicNoLighting;
        }else
        {
            throw std::exception("unknown shader type");
        }
        
        /*transformation*/
        float rx, ry, rz;
        fin >> mis->Translation.x >> mis->Translation.y >> mis->Translation.z;
        fin >> mis->Scale.x >> mis->Scale.y >> mis->Scale.z;
        fin >> rx;
        fin >> ry;
        fin >> rz;
        mis->Rotation.x = XMConvertToRadians(rx);
        mis->Rotation.y = XMConvertToRadians(ry);
        mis->Rotation.z = XMConvertToRadians(rz);

        /*tex overwrite*/
        fin >> diff; //diffuse

        if (diff != "none")
        {
            mis->OverwriteDiffuseMap(diff);
        }

        fin >> norm; //normal

        if (norm != "none")
        {
            mis->OverwriteNormalMap(norm);
        }

        /*auto tex transform for plane, cube*/
        /*error:  x y z depending on rotation?*/
        /*probably delete later*/
        if (mis->GetModelID() == DEFAULT_PLANE || mis->GetModelID() == DEFAULT_CUBE)
        {
            XMMATRIX _r = XMMatrixRotationRollPitchYaw(0, 0, 0);
            XMMATRIX _t = XMMatrixTranslation(0, 0, 0);
            XMMATRIX _s = XMMatrixScaling(mis->Scale.x/4, mis->Scale.z/4, 1.f);
            XMStoreFloat4x4(&mis->TextureTransform, _r * _s * _t);
        }

        modelsStatic.insert(std::make_pair(objID, mis));

    }

}

