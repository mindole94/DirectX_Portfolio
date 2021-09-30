#include "stdafx.h"
#include "TerrainDemo.h"

void TerrainDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(4, 0, 0);
	Context::Get()->GetCamera()->Position(29, 6, -19);
	((Freedom *)Context::Get()->GetCamera())->Speed(20, 2);

	shader = new Shader(L"11_Terrain.fx");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	//terrain->BaseMap(L"Terrain/Dirt3.png");
	terrain->BaseMap(L"Floor.png");
}

void TerrainDemo::Destroy()
{
	SafeDelete(shader);

	SafeDelete(terrain);
}

void TerrainDemo::Update()
{
	terrain->Update();
}

void TerrainDemo::Render()
{
	DebugLine::Get()->RenderLine(Vector3(0, 0, 0), Vector3(0, 100, 0));

	terrain->Render();
}
