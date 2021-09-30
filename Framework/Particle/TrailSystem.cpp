#include "Framework.h"
#include "TrailSystem.h"
#include "Utilities/Xml.h"

TrailSystem::TrailSystem(wstring file, bool bColor)
	: Renderer(L"38_Trail.fxo")
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ReadFile(L"../../_Textures/Trails/" + file + L".xml");

	sMap = shader->AsSRV("TrailMap");
	
	Reset();

	shader->AsScalar("ColorR")->SetFloat(color.r);
	shader->AsScalar("ColorG")->SetFloat(color.g);
	shader->AsScalar("ColorB")->SetFloat(color.b);
	shader->AsScalar("ColorA")->SetFloat(color.a);

	if (bColor == true)
		Pass(3);
	else
		Pass(2);
}

TrailSystem::~TrailSystem()
{
	SafeDelete(map);

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);
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
	}

	indices = new UINT[MAX_TRAIL_NUM];
	for (UINT i = 0; i < MAX_TRAIL_NUM; i++)
	{
		indices[i] = i;
	}

	vertexBuffer = new VertexBuffer(vertices, MAX_TRAIL_NUM, sizeof(VertexTrail), 0, true);
	indexBuffer = new IndexBuffer(indices, MAX_TRAIL_NUM);

	deleteCount = 0;
	currentCount = 0;

	bDelete = false;
}

void TrailSystem::Add(Vector3 start, Vector3 end)
{
	if (currentCount == 0)
	{
		firstStart = start;
		firstEnd = end;

		prevStart = start;
		prevEnd = end;

		currentCount++;
		return;
	}
	
	if (currentCount == 1)
	{
		vertices[0].Position = prevStart;
		vertices[1].Position = prevEnd;

		prevStart = start;
		prevEnd = end;
	}
	else if (currentCount == 2)
	{
		vertices[2].Position = firstStart;
		vertices[3].Position = firstEnd;

		vertices[0].Position = prevStart;
		vertices[1].Position = prevEnd;

		prevStart = start;
		prevEnd = end;
	}
	else
	{
		vertices[(currentCount * 2) - 2].Position = firstStart;
		vertices[(currentCount * 2) - 1].Position = firstEnd;

		for (UINT i = (currentCount * 2) - 3; i > 2;)
		{
			vertices[i].Position = vertices[i - 2].Position;
			i--;
			vertices[i].Position = vertices[i - 2].Position;
			i--;
		}

		D3DXVec3CatmullRom(&vertices[0].Position, &start, &prevStart, &vertices[0].Position, &vertices[0].Position, 0.5f);
		D3DXVec3CatmullRom(&vertices[1].Position, &end, &prevEnd, &vertices[1].Position, &vertices[1].Position, 0.5f);

		D3DXVec3CatmullRom(&vertices[2].Position, &prevStart, &vertices[0].Position, &vertices[2].Position, &vertices[4].Position, 0.5f);
		D3DXVec3CatmullRom(&vertices[3].Position, &prevEnd, &vertices[1].Position, &vertices[3].Position, &vertices[5].Position, 0.5f);

		for (UINT i = (currentCount * 2) - 3; i > 4;)
		{
			D3DXVec3CatmullRom(&vertices[i].Position, &vertices[i - 2].Position, &vertices[i - 2].Position, &vertices[i + 2].Position, &vertices[i + 2].Position, 0.5f);
			i--;
			D3DXVec3CatmullRom(&vertices[i].Position, &vertices[i - 2].Position, &vertices[i - 2].Position, &vertices[i + 2].Position, &vertices[i + 2].Position, 0.5f);
			i--;
		}
	
		prevStart = start;
		prevEnd = end;
	}

	currentCount++;
}

void TrailSystem::Update()
{
	Super::Update();
	Delete();
}

void TrailSystem::Render()
{
	if (currentCount <= 2) return;
	
	SafeDelete(vertexBuffer);
	vertexBuffer = new VertexBuffer(&vertices[0], MAX_TRAIL_NUM, sizeof(VertexTrail));

	if (bDelete == false)
	{
		for (UINT i = 0; i < ((currentCount - 1) * 2) / 2; i++)
		{
			vertices[(i * 2) + 0].Uv = Vector2((float)i / (currentCount - 2), 1.0f);

			vertices[(i * 2) + 1].Uv = Vector2((float)i / (currentCount - 2), 0.0f);
		}
	}
	else
	{
		for (UINT i = 0; i < (activeVertex / 2); i++)
		{
			vertices[(i * 2) + 0].Uv = Vector2((float)(i + deleteCount) / ((activeVertex - 2) / 2), 1.0f);

			vertices[(i * 2) + 1].Uv = Vector2((float)(i + deleteCount) / ((activeVertex - 2) / 2), 0.0f);
		}
	}

	Super::Render();

	sMap->SetResource(map->SRV());


	shader->DrawIndexed(0, Pass(), currentCount > 1 ? ((currentCount - 2) * 2) : 0);
}

void TrailSystem::SetColor(Color color)
{
	this->color = color;

	shader->AsScalar("ColorR")->SetFloat(this->color.r);
	shader->AsScalar("ColorG")->SetFloat(this->color.g);
	shader->AsScalar("ColorB")->SetFloat(this->color.b);
	shader->AsScalar("ColorA")->SetFloat(this->color.a);
}

void TrailSystem::Delete()
{
	if (bDelete == false) 
	{
		activeVertex = (currentCount - 1) * 2;
		return;
	}

	deleteTime += Time::Delta();

	if (deleteTime > 0.001f)
	{
		deleteTime = 0.0f;
		currentCount--;
		deleteCount++;
	}
	
	if (currentCount < 2)
	{
		bDelete = false;
		Reset();
	}

}

void TrailSystem::ReadFile(wstring file)
{
	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLError error = document->LoadFile(String::ToString(file).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();

	Xml::XMLElement* node = root->FirstChildElement();

	wstring textureFile = String::ToWString(node->GetText());
	map = new Texture(L"Trails/" + textureFile);
	
	node = node->NextSiblingElement();
	color.r = node->FloatAttribute("R");
	color.g = node->FloatAttribute("G");
	color.b = node->FloatAttribute("B");
	color.a = node->FloatAttribute("A");
	
	SafeDelete(document);
}
