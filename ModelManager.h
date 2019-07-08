#pragma once

#include "DirectXBase.h"
#include "ModelLoader.h"
#include <string>
#include <map>

class ModelManager
{

public:
    ModelManager(ID3D11Device* dev);
    ~ModelManager();
    bool Add(std::string file);
    Model* Get(std::string id);

private:
    ModelLoader* loader;
    std::map<std::string, Model*> collection;
    ID3D11Device* device;
};