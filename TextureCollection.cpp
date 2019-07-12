#include "TextureCollection.h"
#include "DDSTextureLoader.h"

/*keep device for texture loading*/
TextureCollection::TextureCollection(ID3D11Device* dev)
{
    device = dev;
    defaultID = DEFAULT_NONE;
}

/*release all shader resource views in the destructor*/
TextureCollection::~TextureCollection()
{

    std::map<std::string, ID3D11ShaderResourceView*>::iterator it = collection.begin();

    while (it != collection.end())
    {
        it->second->Release();
        it++;
    }
    collection.clear();
}

/*load texture in file and add it to collection*/
bool TextureCollection::Add(std::string file)
{

    ID3D11ShaderResourceView* srv = 0;

    char id[1024];
    _splitpath_s(file.c_str(), NULL, 0, NULL, 0, id, 1024, NULL, 0);

    /*check if already exists*/

    if (collection.find(id) != collection.end())
    {
        return false;
    }

    /*load*/
    HRESULT hr = DirectX::CreateDDSTextureFromFile(device, std::wstring(file.begin(), file.end()).c_str(), nullptr, &srv);

    if (FAILED(hr))
    {
        throw std::exception("failed to create srv from file");
        return false;
    }

    collection.insert(std::make_pair(id, srv));
    return true;
}

/*access texture with specified id*/
ID3D11ShaderResourceView* TextureCollection::Get(std::string id)
{

    if (collection.find(id) == collection.end())
    {
        if (defaultID == DEFAULT_NONE)
        {
            throw std::invalid_argument("texture not in collection");
            return nullptr;
        }
        else
        {
            return collection[defaultID];
        }
    }

    return collection[id];

}

bool TextureCollection::SetDefaultTexture(std::string id)
{
    if (collection.find(id) == collection.end())
    {
        throw std::exception("can't set default texture");
        return false;
    }

    defaultID = id;
    return true;
}
