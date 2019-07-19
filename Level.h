#pragma once

#include "util.h"
#include "ModelInstanceStatic.h"
#include <fstream>

#define LEVEL_PATH "data/levels/"

class Level
{
public:
    Level(ResourceManager *r);
    ~Level();

    bool LoadLevel(std::string fileName);

    std::map<std::string, ModelInstanceStatic*> modelsStatic;

private:
    void ReadStaticModels(std::ifstream& fin);
    ResourceManager* res = 0;

};