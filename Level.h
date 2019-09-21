#pragma once

#include "util.h"
#include "ModelInstanceStatic.h"
#include <fstream>

class Level
{
public:
    Level(ResourceManager *r);
    ~Level();

    bool LoadLevel(std::string fileName);

    std::map<int, ModelInstanceStatic*> modelsStatic;

private:
    void ReadStaticModels(std::ifstream& fin);
    ResourceManager* res = 0;

};