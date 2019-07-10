#include "ModelCollection.h"

ModelManager::ModelManager(ID3D11Device* dev)
{
    device = dev;
    loader = new ModelLoader();

}

ModelManager::~ModelManager()
{

    delete loader;

    /*clear collection*/
    std::map<std::string, Model*>::iterator it = collection.begin();

    while (it != collection.end())
    {
        delete it->second;
        it++;
    }

}

bool ModelManager::Add(std::string file)
{
    Model* m = new Model(device);
    
    bool r = loader->Load(file, m);

    if (!r)
    {
        return false;
    }

    /* id is file name without extension*/
    collection.insert(std::make_pair(file.substr(0, file.find_last_of(".")), m));

    return true;
}

Model* ModelManager::Get(std::string id)
{
    if (collection.find(id) == collection.end())
    {
        return nullptr;
    }
    
    return collection[id];

}
