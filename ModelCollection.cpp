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

    /* id is file name without extension*/
    char id[128];
    char ext[12];
    _splitpath_s(file.c_str(), NULL, 0, NULL, 0, id, 128, ext, 12);

    if (strcmp(ext, ".bas") == 0)
    {
        return true;
    }

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

    return AddModel(id, m);
}

/*add loaded model to collection, id is not the filename!*/
bool ModelCollection::AddModel(std::string id, Model* m)
{
    /*check if already exists*/
    if (collection.find(id) != collection.end())
    {
        return false;
    }

    m->CreateBuffers();
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

/*set default model which is used when a model is not found in the collection*/
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

/*create a basic cube*/
Model* ModelCollection::CreateCubeModel(float width, float height, float depth)
{
    Model* model = new Model(device);
    Mesh* mesh = new Mesh();
    Material::Standard mat;
    Vertex::PosTexNormalTan v[24];

    float w = width * .5f;
    float h = height * .5f;
    float d = depth * .5f;

    /*define faces*/
    v[0] = Vertex::PosTexNormalTan(XMFLOAT3(-w, -h, -d), XMFLOAT2(0.f, 1.f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
    v[1] = Vertex::PosTexNormalTan(XMFLOAT3(-w, +h, -d), XMFLOAT2(0.f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
    v[2] = Vertex::PosTexNormalTan(XMFLOAT3(+w, +h, -d), XMFLOAT2(1.f, 0.f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));
    v[3] = Vertex::PosTexNormalTan(XMFLOAT3(+w, -h, -d), XMFLOAT2(1.f, 1.f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));

    v[4] = Vertex::PosTexNormalTan(-w, -h, +d, 1.f, 1.f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f);
    v[5] = Vertex::PosTexNormalTan(+w, -h, +d, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f);
    v[6] = Vertex::PosTexNormalTan(+w, +h, +d, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f);
    v[7] = Vertex::PosTexNormalTan(-w, +h, +d, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f);

    v[8] = Vertex::PosTexNormalTan(-w, +h, -d, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    v[9] = Vertex::PosTexNormalTan(-w, +h, +d, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    v[10] = Vertex::PosTexNormalTan(+w, +h, +d, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    v[11] = Vertex::PosTexNormalTan(+w, +h, -d, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f);

    v[12] = Vertex::PosTexNormalTan(-w, -h, -d, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    v[13] = Vertex::PosTexNormalTan(+w, -h, -d, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    v[14] = Vertex::PosTexNormalTan(+w, -h, +d, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f);
    v[15] = Vertex::PosTexNormalTan(-w, -h, +d, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f);

    v[16] = Vertex::PosTexNormalTan(-w, -h, +d, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
    v[17] = Vertex::PosTexNormalTan(-w, +h, +d, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
    v[18] = Vertex::PosTexNormalTan(-w, +h, -d, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
    v[19] = Vertex::PosTexNormalTan(-w, -h, -d, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);

    v[20] = Vertex::PosTexNormalTan(+w, -h, -d, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    v[21] = Vertex::PosTexNormalTan(+w, +h, -d, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    v[22] = Vertex::PosTexNormalTan(+w, +h, +d, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    v[23] = Vertex::PosTexNormalTan(+w, -h, +d, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    
    mesh->hasTextureCoordinates = true;
    mesh->hasTangentu = true;

    mesh->diffuseMapID = "default";

    mesh->vertices.assign(&v[0], &v[24]);

    /*indices*/
    UINT i[36];

    i[0] = 0; i[1] = 1; i[2] = 2;
    i[3] = 0; i[4] = 2; i[5] = 3;
    i[6] = 4; i[7] = 5; i[8] = 6;
    i[9] = 4; i[10] = 6; i[11] = 7;
    i[12] = 8; i[13] = 9; i[14] = 10;
    i[15] = 8; i[16] = 10; i[17] = 11;
    i[18] = 12; i[19] = 13; i[20] = 14;
    i[21] = 12; i[22] = 14; i[23] = 15;
    i[24] = 16; i[25] = 17; i[26] = 18;
    i[27] = 16; i[28] = 18; i[29] = 19;
    i[30] = 20; i[31] = 21; i[32] = 22;
    i[33] = 20; i[34] = 22; i[35] = 23;

    mesh->indices.assign(&i[0], &i[36]);

    /*material*/

    mat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    mat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);

    mesh->material = mat;

    /*mesh in model*/
    model->meshes.clear();
    model->meshes.push_back(mesh);
    model->axisRot = XMMatrixIdentity();

    return model;
}

/*create a basic sphere*/
Model* ModelCollection::CreateSphereModel(float radius, int slices, int stacks)
{
    Model* model = new Model(device);
    Mesh* mesh = new Mesh();
    Material::Standard mat;
                                //pos             //tex     //normal       //tang
    Vertex::PosTexNormalTan top(0.f, radius, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f);
    Vertex::PosTexNormalTan bot(0.f, -radius, 0.f, 0.f, 1.f,0.f, -1.f, 0.f, 1.f, 0.f, 0.f);

    mesh->vertices.clear();
    mesh->indices.clear();

    mesh->vertices.push_back(top);

    float phiStep = XM_PI / stacks;
    float thetaStep = XM_PI * 2.f / slices;


    for (int i = 1; i <= stacks - 1; i++)
    {
        float phi = i * phiStep;

        for (int j = 0; j <= slices; ++j)
        {
            float theta = j * thetaStep;

            Vertex::PosTexNormalTan v;

            v.Pos.x = radius * sinf(phi) * cosf(theta);
            v.Pos.y = radius * cosf(phi);
            v.Pos.z = radius * sinf(phi) * sinf(theta);

            v.TangentU.x = -radius * sinf(phi) * sinf(theta);
            v.TangentU.y = 0.0f;
            v.TangentU.z = +radius * sinf(phi) * cosf(theta);

            XMVECTOR T = XMLoadFloat3(&v.TangentU);
            XMStoreFloat3(&v.TangentU, XMVector3Normalize(T));

            XMVECTOR p = XMLoadFloat3(&v.Pos);
            XMStoreFloat3(&v.Normal, XMVector3Normalize(p));

            v.Tex.x = theta / XM_2PI;
            v.Tex.y = phi / XM_PI;

            mesh->vertices.push_back(v);
        }
    }

    mesh->vertices.push_back(bot);

    //indices

    for (int i = 1; i <= slices; ++i)
    {
        mesh->indices.push_back(0);
        mesh->indices.push_back(i + 1);
        mesh->indices.push_back(i);
    }

    int baseIndex = 1;
    int ringVertexCount = slices + 1;
    for (int i = 0; i < stacks - 2; ++i)
    {
        for (int j = 0; j < slices; ++j)
        {
            mesh->indices.push_back(baseIndex + i * ringVertexCount + j);
            mesh->indices.push_back(baseIndex + i * ringVertexCount + j + 1);
            mesh->indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

            mesh->indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
            mesh->indices.push_back(baseIndex + i * ringVertexCount + j + 1);
            mesh->indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
        }
    }

    int southPoleIndex = (int)mesh->vertices.size() - 1;

    baseIndex = southPoleIndex - ringVertexCount;

    for (int i = 0; i < slices; ++i)
    {
        mesh->indices.push_back(southPoleIndex);
        mesh->indices.push_back(baseIndex + i);
        mesh->indices.push_back(baseIndex + i + 1);
    }

    /*material*/

    mat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    mat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);

    mesh->material = mat;

    mesh->diffuseMapID = "default";

    model->meshes.clear();
    model->meshes.push_back(mesh);
    model->axisRot = XMMatrixIdentity();

    return model;
}

/*create a basic 4 vertices plane which is visible on both sides*/
Model* ModelCollection::CreatePlaneModel(float width, float height)
{
    Model* model = new Model(device);
    Mesh* mesh = new Mesh();
    Material::Standard mat;

    float w = width / 2;
    float h = height / 2;

    /*4 vertices*/

    mesh->vertices.push_back(Vertex::PosTexNormalTan(XMFLOAT3(-w, 0.f, -h),       //pos
                             XMFLOAT2(0.f, 1.f),    //u v coord
                             XMFLOAT3(0.f, 1.f, 0.f),   //normal
                             XMFLOAT3(1.f, 0.f, 0.f))); //tangent
    mesh->vertices.push_back(Vertex::PosTexNormalTan(XMFLOAT3(-w, 0.f, h),       //pos
                             XMFLOAT2(0.f, 0.f),    //u v coord
                             XMFLOAT3(0.f, 1.f, 0.f),   //normal
                             XMFLOAT3(1.f, 0.f, 0.f))); //tangent
    mesh->vertices.push_back(Vertex::PosTexNormalTan(XMFLOAT3(w, 0.f, h),       //pos
                             XMFLOAT2(1.f, 0.f),    //u v coord
                             XMFLOAT3(0.f, 1.f, 0.f),   //normal
                             XMFLOAT3(1.f, 0.f, 0.f))); //tangent
    mesh->vertices.push_back(Vertex::PosTexNormalTan(XMFLOAT3(w, 0.f, -h),       //pos
                             XMFLOAT2(1.f, 1.f),    //u v coord
                             XMFLOAT3(0.f, 1.f, 0.f),   //normal
                             XMFLOAT3(1.f, 0.f, 0.f))); //tangent

    /*indices, plane is visible on both sides*/
    mesh->indices.push_back(0);
    mesh->indices.push_back(1);
    mesh->indices.push_back(2);

    mesh->indices.push_back(0);
    mesh->indices.push_back(2);
    mesh->indices.push_back(3);

    mesh->indices.push_back(0);
    mesh->indices.push_back(2);
    mesh->indices.push_back(1);

    mesh->indices.push_back(0);
    mesh->indices.push_back(3);
    mesh->indices.push_back(2);

    /*material*/
    mat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    mat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);

    mesh->material = mat;

    mesh->diffuseMapID = "default";


    model->meshes.clear();
    model->meshes.push_back(mesh);
    model->axisRot = XMMatrixIdentity();

    return model;
}
