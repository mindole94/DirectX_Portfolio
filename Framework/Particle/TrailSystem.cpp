#include "Framework.h"
#include "TrailSystem.h"
#include "Utilities/Xml.h"

TrailSystem::TrailSystem()
	: Renderer(L"TrailRender.fxo")
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	trailMap = new Texture(L"Trails/Unleashed_Orange.png");
	sTrailMap = shader->AsSRV("TrailMap");

	Reset();
}

TrailSystem::~TrailSystem()
{
	SafeDelete(trailMap);

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);
}

void TrailSystem::Update()
{
	if (trailMap == NULL) return;

	if (bColor == true)	Pass(1);
	else				Pass(0);

	Super::Update();
	Delete();
}

void TrailSystem::Render()
{
	if (position.size() < 2) return;

	int index = position.size() - 1;

	SafeDelete(vertexBuffer);
	vertexBuffer = new VertexBuffer(&vertices[0], index * count * 2, sizeof(VertexTrail));

	for (int i = 0; i < index * count * 2; i++)
	{
		vertices[(i * 2) + 0].Uv = Vector2((float)i / (index * count * 2), 1.0f);
		vertices[(i * 2) + 1].Uv = Vector2((float)i / (index * count * 2), 0.0f);
	}

	Super::Render();

	sTrailMap->SetResource(trailMap->SRV());
	shader->DrawIndexed(0, Pass(), index * count * 2);
}

void TrailSystem::Reset()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);

	vertices = new VertexTrail[MAX_TRAIL_NUM];

	for (UINT i = 0; i < MAX_TRAIL_NUM / 2; i++)
	{
		vertices[(i * 2) + 0].Position = Vector3(0, 0, 0);
		vertices[(i * 2) + 0].Uv = Vector2((float)i / ((MAX_TRAIL_NUM - 2)), 1.0f);

		vertices[(i * 2) + 1].Position = Vector3(0, 0, 0);
		vertices[(i * 2) + 1].Uv = Vector2((float)i / ((MAX_TRAIL_NUM - 2)), 0.0f);

		vertices[(i * 2) + 0].Color = color;
		vertices[(i * 2) + 1].Color = color;
	}

	indices = new UINT[MAX_TRAIL_NUM];
	for (UINT i = 0; i < MAX_TRAIL_NUM; i++)
	{
		indices[i] = i;
	}

	vertexBuffer = new VertexBuffer(vertices, MAX_TRAIL_NUM, sizeof(VertexTrail), 0, true);
	indexBuffer = new IndexBuffer(indices, MAX_TRAIL_NUM);

	position.clear();

	bDelete = false;
}

void TrailSystem::Add(Vector3 start, Vector3 end)
{
	if (position.empty() == true)
	{
		TrailPosition trail;
		trail.Bottom = start;
		trail.Top = end;

		position.push_back(trail);
	}
	else
	{
		TrailPosition trail;
		trail.Bottom = start;
		trail.Top = end;

		position.push_back(trail);

		int index = position.size() - 1;
		int start = (index - 1) * count; //해당 정점의 vertices시작점
		float rate = 0.00f;

		for (int i = start; i < start + count; i++)
		{
			D3DXVec3CatmullRom(&vertices[(i * 2) + 0].Position,
				&position[index - 1].Bottom, &position[index - 1].Bottom,
				&position[index].Bottom, &position[index].Bottom, rate);

			D3DXVec3CatmullRom(&vertices[(i * 2) + 1].Position,
				&position[index - 1].Top, &position[index - 1].Top,
				&position[index].Top, &position[index].Top, rate);

			rate += (1 / count);
		}


		//@brief : 정점 보정
		for (int i = 1; i < start + count - 2; i++)
		{
			int number = i * 2;

			D3DXVec3CatmullRom(&vertices[number + 0].Position,
				&vertices[number + 0 - 2].Position, &vertices[number + 0 - 2].Position,
				&vertices[number + 0 + 2].Position, &vertices[number + 0 + 2].Position, 0.5f);

			D3DXVec3CatmullRom(&vertices[number + 1].Position,
				&vertices[number + 1 - 2].Position, &vertices[number + 1 - 2].Position,
				&vertices[number + 1 + 2].Position, &vertices[number + 1 + 2].Position, 0.5f);
		}

		for (int i = 1; i < start + count - 2; i++)
		{
			int number = i * 2;

			D3DXVec3CatmullRom(&vertices[number + 0].Position,
				&vertices[number + 0 - 2].Position, &vertices[number + 0 - 2].Position,
				&vertices[number + 0 + 2].Position, &vertices[number + 0 + 2].Position, 0.5f);

			D3DXVec3CatmullRom(&vertices[number + 1].Position,
				&vertices[number + 1 - 2].Position, &vertices[number + 1 - 2].Position,
				&vertices[number + 1 + 2].Position, &vertices[number + 1 + 2].Position, 0.5f);
		}
	}
}

void TrailSystem::TrailMap(string file)
{
	TrailMap(String::ToWString(file));
}

void TrailSystem::TrailMap(wstring file)
{
	if (trailMap != NULL)
		SafeDelete(trailMap);

	trailMap = new Texture(file);
}

void TrailSystem::Delete()
{
	if (bDelete == false)
		return;

	deleteTime += Time::Delta();

	if (deleteTime < 0.005f)
		return;

	deleteTime = 0.0f;
	if(position.empty() == false)
		position.erase(position.begin());

	if (position.size() < 1)
	{
		bDelete = false;
		Reset();
	}
}
