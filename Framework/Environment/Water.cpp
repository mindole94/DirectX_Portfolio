#include "Framework.h"
#include "Water.h"

Water::Water(InitializeDesc& initDesc)
	: Renderer(initDesc.Shader)
	, initDesc(initDesc)
{
	vertexCount = 4;
	indexCount = 6;

	VertexTexture vertices[4];
	vertices[0].Position = Vector3(-initDesc.Radius, 0.0f, -initDesc.Radius);
	vertices[1].Position = Vector3(-initDesc.Radius, 0.0f, +initDesc.Radius);
	vertices[2].Position = Vector3(+initDesc.Radius, 0.0f, -initDesc.Radius);
	vertices[3].Position = Vector3(+initDesc.Radius, 0.0f, +initDesc.Radius);

	vertices[0].Uv = Vector2(0, 1);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(1, 1);
	vertices[3].Uv = Vector2(1, 0);

	UINT indices[6] = { 0, 1, 2, 2, 1, 3 };

	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexTexture));
	indexBuffer = new IndexBuffer(indices, indexCount);

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Water");

	this->initDesc.Width = initDesc.Width > 0 ? initDesc.Width : (UINT)D3D::Width();
	this->initDesc.Height = initDesc.Height > 0 ? initDesc.Height : (UINT)D3D::Height();

	reflection = new Reflection(shader, transform, this->initDesc.Width, this->initDesc.Height);
	refraction = new Refraction(shader, L"Environment/Wave.dds", this->initDesc.Width, this->initDesc.Height);

	heightMap = new Texture(initDesc.HeightFile);
	sHeightMap = shader->AsSRV("HeightMap");

	waterMap = new Texture(L"Environment/Water.png");
	shader->AsSRV("WaterMap")->SetResource(waterMap->SRV());
}

Water::~Water()
{
	SafeDelete(buffer);

	SafeDelete(reflection);
	SafeDelete(refraction);

	SafeDelete(heightMap);
}

void Water::Update()
{
	Super::Update();

	desc.WaveTranslation += WaveSpeed * Time::Delta();
	if (desc.WaveTranslation > 1.0f)
		desc.WaveTranslation -= 1.0f;

	reflection->Update();
	refraction->Update();
}

void Water::PreRender_Reflection()
{
	Vector3 position;
	transform->Position(&position);
	
	Plane plane = Plane(0, 1, 0, -position.y);
	Context::Get()->Clipping() = plane;
	
	reflection->PreRender();
}

void Water::PreRender_Refraction()
{
	Vector3 position;
	transform->Position(&position);
	
	Plane plane = Plane(0, -1, 0, position.y + 0.1f);
	Context::Get()->Clipping() = plane;

	refraction->PreRender();
}

void Water::PreRender_End()
{
	Plane plane = Plane(0, 0, 0, 0);
	Context::Get()->Clipping() = plane;
}

void Water::Render()
{
	Super::Render();

	reflection->Render();
	refraction->Render();

	desc.TerrainWidth = (float)heightMap->GetWidth();
	desc.TerrainHeight = (float)heightMap->GetHeight();
	
	Vector3 scale;
	GetTransform()->Scale(&scale);

	desc.WaterWidth = scale.x * initDesc.Radius * 2;
	desc.WaterHeight = scale.y * initDesc.Radius * 2;
	
	Vector3 position;
	GetTransform()->Position(&position);
	
	desc.WaterPositionY = position.y;
	
	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	sHeightMap->SetResource(heightMap->SRV());

	shader->DrawIndexed(0, Pass(), indexCount);
}