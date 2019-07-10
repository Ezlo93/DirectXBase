#include "Model.h"


Model::Model(ID3D11Device* dev)
{
    device = dev;
}

Model::~Model()
{
    for (auto& i : meshes)
    {
        delete i;
    }
    
}

/*draw all meshes of the model*/
void Model::Draw(ID3D11DeviceContext* context)
{
    for (auto& m : meshes)
    {
        m->Draw(context);
    }

}

/*create vertex and indexbuffers for all meshes of the model*/
void Model::CreateBuffers()
{

    for (auto& m : meshes)
    {

        m->createBuffers(device);

    }

}
