#pragma once
#include "util.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "Shader.h"

#define DEFAULT_PLANE "defaultPlane"
#define DEFAULT_SPHERE "defaultSphere"
#define DEFAULT_CUBE "defaultCube"

class ModelInstanceStatic
{
public:
    ModelInstanceStatic(ResourceManager* r, std::string id);
    ~ModelInstanceStatic();

    void SetModelID(std::string id)
    {
        modelID = id;
    }
    std::string GetModelID()
    {
        return modelID;
    }

    XMFLOAT4X4 getWorld()
    {
        return World;
    }

    void Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera *c);
    void OverwriteDiffuseMap(std::string id);
    void OverwriteNormalMap(std::string id);

    /*public properties*/
    XMFLOAT3 Translation, Rotation, Scale;
    XMFLOAT4X4 TextureTransform;

    UShader::UsedShader usedShader;
    UTech::UsedTechnique usedTechnique;
private:

    bool useOverwriteDiffuse;
    bool useOverwriteNormalMap;
    std::string ovrwrTex;
    std::string ovrwrNrm;

    XMFLOAT4X4 World;
    ResourceManager* resources = 0;

    std::string modelID;
};