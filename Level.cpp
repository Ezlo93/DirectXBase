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

    json lvl;
    try
    {
        lvl = json::parse(fin);
    }
    catch (json::parse_error& e)
    {
        throw std::exception(e.what());
        return false;
    }
    fin.close();

    /*read different parts*/
    ReadStaticModels(lvl);

   

    return true;
}

void Level::ReadStaticModels(json& j)
{
    
    for (auto& i : j["static"])
    {
        /*check double id*/
        if (!exists(i, "id"))
        {
            throw std::exception("static model without id");
            return;
        }

        if (modelsStatic.find(i["id"]) != modelsStatic.end())
        {
            throw std::exception("id already exists");
            return;
        }

        ModelInstanceStatic* mis = new ModelInstanceStatic(res, i["model"]);

        /*shader*/
        std::string shaderString = i["shader"];
        if (shaderString == "basictexture")
        {
            mis->usedShader = UShader::Basic;
            mis->usedTechnique = UTech::Basic;
        }
        else if (shaderString == "basicnotexture")
        {
            mis->usedShader = UShader::Basic;
            mis->usedTechnique = UTech::BasicNoTexture;
        }
        else if (shaderString == "basicnolighting")
        {
            mis->usedShader = UShader::Basic;
            mis->usedTechnique = UTech::BasicNoLighting;
        }
        else if (shaderString == "normalmap")
        {
            mis->usedShader = UShader::Normal;
            mis->usedTechnique = UTech::NormalTech;
        }
        else if (shaderString == "onlyshadow")
        {
            mis->usedShader = UShader::Basic;
            mis->usedTechnique = UTech::BasicOnlyShadow;
        }
        else
        {
            throw std::exception("unknown shader type");
        }

        /*transforms*/
        mis->Translation.x = i["position"][0];
        mis->Translation.y = i["position"][1];
        mis->Translation.z = i["position"][2];

        mis->Scale.x = i["scale"][0];
        mis->Scale.y = i["scale"][1];
        mis->Scale.z = i["scale"][2];

        mis->Rotation.x = XMConvertToRadians(i["rotation"][0]);
        mis->Rotation.y = XMConvertToRadians(i["rotation"][1]);
        mis->Rotation.z = XMConvertToRadians(i["rotation"][2]);

        /*overwrites*/
        if (exists(i, "overwriteTexture"))
        {
            mis->OverwriteDiffuseMap(i["overwriteTexture"]);
        }

        if (exists(i, "overwriteNormal"))
        {
            mis->OverwriteNormalMap(i["overwriteNormal"]);
        }

        if (mis->GetModelID() == DEFAULT_PLANE || mis->GetModelID() == DEFAULT_CUBE)
        {
            XMMATRIX _r = XMMatrixRotationRollPitchYaw(0, 0, 0);
            XMMATRIX _t = XMMatrixTranslation(0, 0, 0);
            XMMATRIX _s = XMMatrixScaling(mis->Scale.x / 4, mis->Scale.z / 4, 1.f);
            XMStoreFloat4x4(&mis->TextureTransform, _r * _s * _t);
        }

        /*other*/
        if (exists(i, "hasCollision"))
        {
            mis->hasCollision = i["hasCollision"];
        }

        if (exists(i, "isInvisible"))
        {
            mis->isInvisible = i["isInvisible"];
        }

        if (exists(i, "castsShadow"))
        {
            mis->castsShadow = i["castsShadow"];
        }

        modelsStatic.insert(std::make_pair(i["id"], mis));
    }

}

