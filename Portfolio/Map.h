#pragma once

class Map
{
public:
	Map(Shader* shader);
	~Map();

	void Update();
	void PreRender();
	void PreRender_Reflection_Water();
	void PreRender_Refraction_Water();
	void PreRender_Reflection_Sea();
	void PreRender_Refraction_Sea();
	void Render();

	void GetPlayerPosition(Vector3 vec) { playerPos = vec; }
	
	Sky* GetSky() { return sky; }
	SeaLOD* GetSea() { return sea; }
	Water* GetWater() { return water; }
	TerrainLOD* GetTerrain() { return terrain; }

	void TerrainPass(UINT pass) { terrainPass = pass; }
	UINT TerrainPass() { return terrainPass; }

	void SeaPass(UINT pass) { seaPass = pass; }
	UINT SeaPass() { return seaPass; }

	void BSea(bool b) { this->bSea = b; }
	bool BSea() { return bSea; }

	void SetPlantShake(float shake) { this->plantShake = shake; }
	float GetPlantShake() { return plantShake; }

private:
	void LoadPlants(wstring file);
	void LoadTree(wstring file);

private:
	Shader* shader;

	Sky* sky;
	SeaLOD* sea;
	Water* water;
	TerrainLOD* terrain;
	TerrainLOD* floor;
	Billboard* billboard;

	PerFrame* perFrame;

	vector<ModelRender *> trees;

private:
	Vector3 playerPos;
	UINT terrainPass = 8;
	UINT seaPass = 24;

	bool bSea = false;

	float plantShake = 0.0f;
};