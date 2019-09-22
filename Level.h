#pragma once

#include "util.h"
#include "ModelInstanceStatic.h"
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

class Level
{
public:
    Level(ResourceManager *r);
    ~Level();

    bool LoadLevel(std::string fileName);

    std::map<int, ModelInstanceStatic*> modelsStatic;

private:
    void ReadStaticModels(json& j);
    ResourceManager* res = 0;

};