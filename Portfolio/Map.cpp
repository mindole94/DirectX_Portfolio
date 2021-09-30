#include "stdafx.h"
#include "Map.h"
#include "Utilities/Xml.h"

Map::Map(Shader * shader)
	: shader(shader)
{
	sky = new Sky(shader);
	sky->RealTime(false, Math::PI - 1e-6f);
	//sky->RealTime(true, Math::PI - 1e-6f, 0.01f);

	wstring mapFile = L"Map1/Save7.dds";

	terrain = new TerrainLOD(shader, mapFile);
	terrain->BaseMap(L"Terrain/Dirt3.png");
	//terrain->BaseMap(L"Terrain/Forest Floor.jpg");
	
	sea = new SeaLOD(shader, (UINT)terrain->GetWidth(), (UINT)terrain->GetHeight(), terrain->GetCellspacing());

	static float rate = 250.0f;

	sea->SetCullBackwardRate(rate);
	terrain->SetCullBackwardRate(rate);

	LoadPlants(L"Save7.plants");
	LoadTree(L"Save7.models");

	water = new Water
	(
		Water::InitializeDesc
		{
			shader, terrain->GetHeight() * 0.5f, mapFile, 0, 0
		}
	);
	water->GetTransform()->Position(0.0f, 90.0f, 0.0f);
	water->SetWaterPositionYMin(80.0f);
	water->SetWaterRangeUV(Color(0.28f, 0.44f, 0.31, 0.43f));

	perFrame = new PerFrame(shader);

	floor = new TerrainLOD(shader, L"Map1/Floor.dds", 2.0f);
	floor->BaseMap(L"Terrain/Dirt3.png");
}

Map::~Map()
{
	SafeDelete(floor);

	SafeDelete(perFrame);
	SafeDelete(billboard);
	SafeDelete(terrain);
	SafeDelete(sea);
	SafeDelete(sky);
}

void Map::Update()
{
	sky->GetPlayerPosition(playerPos);

	Plane plane[6];
	perFrame->GetFrustumPlanes(plane);
	terrain->GetCullingPlanes(plane);
	floor->GetCullingPlanes(plane);
	if(bSea)
		sea->GetCullingPlanes(plane);

	perFrame->Update();

	sky->Update();
	if (bSea)
		sea->Update();
	terrain->Update();
	water->Update();
	floor->Update();

	plantShake += Time::Delta() * 0.5f;
	plantShake = fmodf(plantShake, 2.0f * Math::PI);
	shader->AsScalar("plantShake")->SetFloat(plantShake);

	for (ModelRender* temp : trees)
		temp->Update();

	billboard->Update();
}

void Map::PreRender()
{
	for (ModelRender* temp : trees)
	{
		temp->Pass(1);
		temp->Render();
	}

	billboard->Pass(3);
	billboard->Render();

	sky->PreRender();
}

void Map::PreRender_Reflection_Water()
{
	water->PreRender_Reflection();

	sky->Pass(19);
	sky->Render();

	for (ModelRender* temp : trees)
	{
		temp->Pass(15);
		temp->Render();
	}

	billboard->Pass(17);
	billboard->Render();

	terrain->Pass(18);
	terrain->Render();
}

void Map::PreRender_Refraction_Water()
{
	water->PreRender_Refraction();

	sky->Pass(10);
	sky->Render();

	for (ModelRender* temp : trees)
	{
		temp->Pass(5);
		temp->Render();
	}

	billboard->Pass(7);
	billboard->Render();

	terrain->Pass(terrainPass);
	terrain->Render();
}

void Map::PreRender_Reflection_Sea()
{
	sea->PreRender_Reflection();

	sky->Pass(19);
	sky->Render();

	terrain->Pass(18);
	terrain->Render();

	floor->Pass(18);
	floor->Render();
}

void Map::PreRender_Refraction_Sea()
{
	sea->PreRender_Refraction();

	sky->Pass(10);
	sky->Render();

	terrain->Pass(terrainPass);
	terrain->Render();

	floor->Pass(terrainPass);
	floor->Render();
}

void Map::Render()
{
	sky->Pass(10);
	sky->Render();

	billboard->Pass(7);
	billboard->Render();

	for (ModelRender* temp : trees)
	{
		temp->Pass(5);
		temp->Render();
	}

	terrain->Pass(terrainPass);
	terrain->Render();

	water->Pass(23);
	water->Render();

	if (bSea)
	{
		sea->Pass(seaPass);
		sea->Render();
	}

	perFrame->Render();
}

void Map::LoadPlants(wstring file)
{
	file = L"../../_Data/Map1/" + file;
	billboard = new Billboard(shader);

	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLError error = document->LoadFile(String::ToString(file).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();
	Xml::XMLElement* plantNode = root->FirstChildElement();

	UINT index = 0;
	do
	{
		Xml::XMLElement* node = NULL;

		node = plantNode->FirstChildElement();
		wstring wstr = String::ToWString(node->GetText());
		billboard->AddTexture(L"Terrain/" + wstr);

		node = node->NextSiblingElement();
		UINT count = node->IntText();

		Vector3 pos;
		float scale;
		float space = terrain->GetCellSpacing();
		for (UINT i = 0; i < count; i++)
		{
			node = node->NextSiblingElement();

			pos.x = node->FloatAttribute("X");
			pos.y = node->FloatAttribute("Y");
			pos.z = node->FloatAttribute("Z");

			pos.x -= 128.0f;
			pos.z -= 128.0f;
			pos.x *= space;
			pos.z *= -space;

			node = node->NextSiblingElement();
			scale = node->FloatText();
			float ran = Math::Random(0.5f, 1.0f);
			scale *= ran;
			pos.y = terrain->GetHeight(pos) + scale * 0.5f;
			billboard->Add(pos, Vector2(scale, scale), index);
		}
		

		index++;
		plantNode = plantNode->NextSiblingElement();

	} while (plantNode != NULL);
}

void Map::LoadTree(wstring file)
{
	file = L"../../_Data/Map1/" + file;

	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLError error = document->LoadFile(String::ToString(file).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();
	Xml::XMLElement* treeNode = root->FirstChildElement();

	float space = terrain->GetCellSpacing();

	do
	{
		Xml::XMLElement* node = NULL;

		node = treeNode->FirstChildElement();
		wstring wstr = String::ToWString(node->GetText());
		ModelRender* tree = new ModelRender(shader);
		tree->ReadMesh(wstr + L"/" + wstr);
		tree->ReadMaterial(wstr + L"/" + wstr);

		trees.push_back(tree);

		node = node->NextSiblingElement();
		UINT count = node->IntText();
		
		for (UINT i = 0; i < count; i++)
		{
			Vector3 pos, rot, scale;
			node = node->NextSiblingElement();
			pos.x = node->FloatAttribute("X");
			pos.y = node->FloatAttribute("Y");
			pos.z = node->FloatAttribute("Z");

			node = node->NextSiblingElement();
			rot.x = node->FloatAttribute("X");
			rot.y = node->FloatAttribute("Y");
			rot.z = node->FloatAttribute("Z");

			node = node->NextSiblingElement();
			scale.x = node->FloatAttribute("X");
			scale.y = node->FloatAttribute("Y");
			scale.z = node->FloatAttribute("Z");
			pos.x -= 128.0f;
			pos.z -= 128.0f;
			pos.x *= space;
			pos.z *= -space;
			pos.y = terrain->GetHeight(pos) - 1.0f;
			scale *= 5.0f;

			Transform* transform = tree->AddTransform();
			transform->Position(pos);
			transform->Scale(scale);
			transform->Rotation(rot);
			tree->UpdateTransforms();
		}

		treeNode = treeNode->NextSiblingElement();

	} while (treeNode != NULL);
}
