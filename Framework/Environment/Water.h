#pragma once

class Water : public Renderer
{
public:
	struct InitializeDesc;

public:
	Water(InitializeDesc& initDesc);
	~Water();

	void Update();

	void PreRender_Reflection();
	void PreRender_Refraction();
	void PreRender_End();
	void Render();

	void SetWaterPositionYMin(float min) { desc.WaterPositionYMin = min; }

	Color GetWaterRangeUV() { return desc.RangeUV; }
	void SetWaterRangeUV(Color value) { desc.RangeUV = value; }

public:
	struct InitializeDesc
	{
		Shader* Shader;
		float Radius;
		wstring HeightFile;
		UINT Width = 0;
		UINT Height = 0;
	} initDesc;

private:
	struct Desc
	{
		Color RefractionColor = Color(0.2f, 0.3f, 1.0f, 1.0f);

		Vector2 NormalMapTile = Vector2(4, 8);
		float WaveTranslation = 0.0f;
		float WaveScale = 0.05f;

		float Shininess = 50.0f;
		float Alpha = 0.5f;
		float WaterPositionY;
		float WaterPositionYMin = 0.0f;

		float TerrainWidth;
		float TerrainHeight;
		float WaterWidth;
		float WaterHeight;

		Color RangeUV;//minX, maxX, minY, maxY
	} desc;

private:
	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	class Reflection* reflection;
	class Refraction* refraction;

	Texture* heightMap;
	ID3DX11EffectShaderResourceVariable* sHeightMap;

	Texture* waterMap;
	float WaveSpeed = 0.06f;
};