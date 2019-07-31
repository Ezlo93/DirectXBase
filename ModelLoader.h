#pragma once

#include "Model.h"

class ModelLoader
{

public:
    ModelLoader();
    ~ModelLoader();

    bool LoadB3D(const std::string& fileName, Model* m);
    bool LoadBas(const std::string fileName, Model* m);

private:


};