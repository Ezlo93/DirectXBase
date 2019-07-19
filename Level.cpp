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
    int objCount;
    std::string objID, mID, diff, norm;

    fin >> objCount;

    ASSERT(objCount > 0);


    for (int i = 0; i < objCount; i++)
    {
        fin >> objID;
        fin >> mID; //modelID

        ModelInstanceStatic* mis = new ModelInstanceStatic(res, mID);

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

        modelsStatic.insert(std::make_pair(objID, mis));

    }

}

