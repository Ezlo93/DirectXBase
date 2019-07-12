#include "ModelCollection.h"

ModelCollection::ModelCollection(ID3D11Device* dev)
{
    device = dev;
    loader = new ModelLoader();

    defaultID = DEFAULT_NONE;
}

/*clear collection*/
ModelCollection::~ModelCollection()
{

    delete loader;

    /*clear collection*/
    std::map<std::string, Model*>::iterator it = collection.begin();

    while (it != collection.end())
    {
        delete it->second;
        it++;
    }
    collection.clear();
}

/*load model in file and add it to the collection*/
bool ModelCollection::Add(std::string file)
{
    Model *m;

    char id[1024];
    _splitpath_s(file.c_str(), NULL, 0, NULL, 0, id, 1024, NULL, 0);

    /*check if already exists*/
    if (collection.find(id) != collection.end())
    {
        return false;
    }

    /*load*/
    m = new Model(device);

    if (!loader->Load(file, m))
    {
        throw std::exception("failed to load model");
        return false;
    }
    m->CreateBuffers();
    /* id is file name without extension*/
    collection.insert(std::make_pair(id, m));

    return true;
}

/*return the model with the specified id if it's in the collection*/
Model* ModelCollection::Get(std::string id)
{
    if (collection.find(id) == collection.end())
    {
        if (defaultID == DEFAULT_NONE)
        {
            throw std::invalid_argument("model not in collection");
            return nullptr;
        }
        else
        {
            return collection[defaultID];
        }
        
    }
    
    return collection[id];

}


bool ModelCollection::SetDefaultModel(std::string id)
{
    if (collection.find(id) == collection.end())
    {
        throw std::exception("can't set default model");
        return false;
    }

    defaultID = id;
    return true;
}