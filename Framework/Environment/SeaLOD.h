#pragma once

class SeaLOD : public Renderer
{
public:
	SeaLOD(Shader* shader, UINT width = 0, UINT height = 0, float cellSpace = 1.0f);
	~SeaLOD();

	void Update();
	void Render();

	float GetWidth();//실제 크기
	float GetHeight();//실제 크기

	void SeaMap(wstring file);

	void Position(float x, float y, float z);
	void Position(Vector3& vec);
	void Position(Vector3* vec);

	void GetCullingPlanes(Plane* planes);
	void SetCullBackwardRate(float rate) { this->backwardRate = rate; }

	/*float GetWaveScale() { return waveScale; }
	void SetWaveScale(float scale) { waveScale = scale; }*/

	Vector3 GetWavePosition() { return desc.WavePosition; }
	void SetWavePosition(Vector3 vec) { desc.WavePosition = vec; }

	void PreRender_Reflection();
	void PreRender_Refraction();
	void PreRender_End();

private:
	void CalcVertexBounds();
	void CreatePatchVertex();
	void CreatePatchIndex();

private:
	struct VertexSeaLOD
	{
		Vector3 Position;
		Vector2 Uv;
		Vector2 Bound;
	};

private:
	struct Desc
	{
		Vector2 Distance = Vector2(1, 1000);
		Vector2 TessFactor = Vector2(1, 64);

		float Cellspacing = 16.0f;
		//float Cellspacing = 8.0f;
		float CellspacingU;
		float CellspacingV;
		float WaveScale = 3.0f;

		Plane CullPlane[6];

		Vector3 WavePosition = Vector3(0, 35, 0);
		float Padding;
	} desc;

private:
	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	UINT cellsPerPatch = 32;//패치당 크기 ex)256*256맵이면 한변의 길이 256 -> 한패치의 크기 32 -> 256 / 32 = 8칸

	UINT width = 1024;
	UINT height = 512;
	UINT patchWidth, patchHeight;//patch의 갯수

	Vector2* bounds;

	Texture* seaMap = NULL;
	ID3DX11EffectShaderResourceVariable* sSeaMap;

	Material* material;

	Plane planes[6];

	class Reflection2* reflection;
	class Refraction2* refraction;

	float backwardRate = 100.0f;
};