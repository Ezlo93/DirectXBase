#pragma once

#include "util.h"
#include "ModelLoader.h"

class ModelCollection
{

public:
    ModelCollection(ID3D11Device* dev);
    ~ModelCollection();
    bool Add(std::string file);
    bool AddModel(std::string id, Model* m);
    Model* Get(std::string id);
    bool SetDefaultModel(std::string id);

    Model* CreateCubeModel(float width, float height, float depth);
    Model* CreateSphereModel(float radius, int slices, int stacks);
    Model* CreatePlaneModel(float width, float height);

private:
    ModelLoader* loader;
    std::map<std::string, Model*> collection;
    ID3D11Device* device;
    std::string defaultID;
};