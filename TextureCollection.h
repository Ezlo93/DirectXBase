#pragma once

#include <string>
#include <map>
#include <d3d11.h>

class TextureCollection
{
public:
    TextureCollection(ID3D11Device* dev);
    ~TextureCollection();

    bool Add(std::string file);
    ID3D11ShaderResourceView* Get(std::string id);

private:
    std::map<std::string, ID3D11ShaderResourceView*> collection;
    ID3D11Device* device;

};