#pragma once

#include "Model.h"
#pragma comment(lib, "assimp-vc140-mt.lib")
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <assimp\material.h>
#include <string>

class ModelLoader
{

public:
    ModelLoader();
    ~ModelLoader();

    bool Load(const std::string fileName, Model* m);

private:


};