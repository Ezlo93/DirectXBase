#pragma once
#include "util.h"
#include "Camera.h"
#include "ResourceManager.h"
#include "Shader.h"

class ModelInstanceStatic
{
public:
    ModelInstanceStatic(ID3D11Device* d, ID3D11DeviceContext* c, ResourceManager* r, std::string id);
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

    void Draw(Camera *c);

    /*public properties*/
    XMFLOAT3 Translation, Rotation, Scale;
    XMFLOAT4X4 TextureTransform;
    UShader::UsedShader usedShader;
    UTech::UsedTechnique usedTechnique;
private:
    XMFLOAT4X4 World;
    ID3D11Device* device = 0;
    ID3D11DeviceContext* deviceContext = 0;
    ResourceManager* resources = 0;

    std::string modelID;
};