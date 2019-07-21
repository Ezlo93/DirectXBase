#include "Model.h"


Model::Model(ID3D11Device* dev)
{
    device = dev;
    axisRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90), 0.f, 0.f);
}

Model::~Model()
{
    for (auto& i : meshes)
    {
        delete i;
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
