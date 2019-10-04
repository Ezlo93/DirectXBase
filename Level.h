#pragma once

#include "util.h"
#include "ModelInstanceStatic.h"
#include "ParticleSystem.h"
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

class Level
{
public:
    Level(ResourceManager* r, ID3D11Device* d, ID3D11DeviceContext* c);
    ~Level();

    bool LoadLevel(std::string fileName);

    void Update(float deltaTime);
    void Reset();

    std::map<int, ModelInstanceStatic*> modelsStatic;
    std::map<int, ParticleSystem*> particleSystems;

private:
    void ReadStaticModels(const json& j);
    void ReadParticleSystems(const json& j);
    ResourceManager* res = 0;
    ID3D11Device* device = 0;
    ID3D11DeviceContext* context = 0;
    float totalTime = 0.f;
};

static bool exists(const nlohmann::json& j, const std::string& key)
{
    return j.find(key) != j.end();
}