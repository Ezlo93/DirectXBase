#pragma once

#include "util.h"
#include "ModelLoader.h"

#define DEFAULT_NONE "!none!"

class ModelCollection
{

public:
    ModelCollection(ID3D11Device* dev);
    ~ModelCollection();
    bool Add(std::string file);
    Model* Get(std::string id);
    bool SetDefaultModel(std::string id);

private:
    ModelLoader* loader;
    std::map<std::string, Model*> collection;
    ID3D11Device* device;
    std::string defaultID;
};