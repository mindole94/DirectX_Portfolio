#include "stdafx.h"
#include "TerrainRaycastDemo.h"

void TerrainRaycastDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(15, 0, 0);
	Context::Get()->GetCamera()->Position(8, 5, -10);
	((Freedom *)Context::Get()->GetCamera())->Speed(20, 2);

	shader = new Shader(L"11_Terrain.fx");

	terrain = new Terrain(shader, L"Terrain/Black32.png");
	terrain->BaseMap(L"Terrain/Dirt3.png");
}

void TerrainRaycastDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(terrain);
}

void TerrainRaycastDemo::Update()
{
	Vector3 position = terrain->GetRaycastPosition();

	string str = "";
	str += to_string(position.x) + ",";
	str += to_string(position.y) + ",";
	str += to_string(position.z);

	Gui::Get()->RenderText(Vector2(10, 60), Color(1, 0, 0, 1), str);


	terrain->Update();
}

void TerrainRaycastDemo::Render()
{
	terrain->Render();
}
