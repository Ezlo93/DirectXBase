#pragma once

#include <string>
#include <map>
#include <d3d11.h>

#define DEFAULT_NONE "!none!"

class TextureCollection
{
public:
    TextureCollection(ID3D11Device* dev);
    ~TextureCollection();

    bool Add(std::string file);
    ID3D11ShaderResourceView* Get(std::string id);
    bool SetDefaultTexture(std::string id);

private:
    std::map<std::string, ID3D11ShaderResourceView*> collection;
    ID3D11Device* device;
    std::string defaultID;
};