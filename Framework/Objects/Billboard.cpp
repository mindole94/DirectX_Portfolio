#include "Framework.h"
#include "Billboard.h"

Billboard::Billboard(Shader * shader)
	: Renderer(shader)
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	sDiffuseMap = shader->AsSRV("BillboardMaps");
}

Billboard::~Billboard()
{
	SafeDelete(textureArray);
}

void Billboard::AddTexture(wstring file)
{
	SafeDelete(textureArray);

	textureFiles.push_back(file);
}

void Billboard::Add(Vector3 & position, Vector2 & scale, UINT mapIndex)
{
	VertexBillboard vertex =
	{
		position, scale, mapIndex
	};

	vertices.push_back(vertex);
}


void Billboard::Update()
{
	Super::Update();
}

void Billboard::Render()
{
	if (textureFiles.size() > 0 && textureArray == NULL)
		textureArray = new TextureArray(textureFiles);

	if (vertexCount != vertices.size())
	{
		vertexCount = vertices.size();

		SafeDelete(vertexBuffer);
		vertexBuffer = new VertexBuffer(&vertices[0], vertices.size(), sizeof(VertexBillboard));
	}

	Super::Render();

	sDiffuseMap->SetResource(textureArray->SRV());
	shader->Draw(0, Pass(), vertexCount);
}

void Billboard::PreRender_Reflection()
{

}

void Billboard::GetCullingPlanes(Plane * planes)
{
	memcpy(this->planes, planes, sizeof(Plane) * 6);
}
