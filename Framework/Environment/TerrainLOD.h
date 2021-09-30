#pragma once

class TerrainLOD : public Renderer
{
public:
	//TerrainLOD(wstring heightFile);
	TerrainLOD(Shader * shader, wstring heightFile);
	TerrainLOD(Shader * shader, wstring heightFile, float scale);
	~TerrainLOD();

	void Update();
	void Render();

	float GetWidth();//실제 크기
	float GetHeight();//실제 크기
	float GetCellspacing() { return desc.Cellspacing; }

	void BaseMap(wstring file);

	float GetHeight(Vector3 position);
	float GetCellSpacing() { return desc.Cellspacing; }

	void GetCullingPlanes(Plane* planes);

	void SetCullBackwardRate(float rate) { this->backwardRate = rate; }

private:
	void CalcVertexBounds();
	void CreatePatchVertex();
	void CreatePatchIndex();

private:
	struct VertexTerrainLOD
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

		//float Cellspacing = 16.0f; //한칸의 크기
		float Cellspacing = 8.0f; //한칸의 크기
		float CellspacingU;
		float CellspacingV;
		float HeightScale = 1.0f; //높이 크기

		Plane CullPlane[6];
	} desc;

private:
	UINT cellsPerPatch = 32;//패치당 크기 ex)256*256맵이면 한변의 길이 256 -> 한패치의 크기 32 -> 256 / 32 = 8칸

	UINT width, height;
	UINT patchWidth, patchHeight;//patch의 갯수

	float* heights;
	Vector2* bounds;

	Texture* heightMap;
	ID3DX11EffectShaderResourceVariable* sHeightMap;

	Texture* baseMap = NULL;
	ID3DX11EffectShaderResourceVariable* sBaseMap;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	Material* material;

	Vector3* saveVertices;

	Plane planes[6];

	float backwardRate = 100.0f;//카메라 컬링 조정
};