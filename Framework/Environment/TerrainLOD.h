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

	float GetWidth();//���� ũ��
	float GetHeight();//���� ũ��
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

		//float Cellspacing = 16.0f; //��ĭ�� ũ��
		float Cellspacing = 8.0f; //��ĭ�� ũ��
		float CellspacingU;
		float CellspacingV;
		float HeightScale = 1.0f; //���� ũ��

		Plane CullPlane[6];
	} desc;

private:
	UINT cellsPerPatch = 32;//��ġ�� ũ�� ex)256*256���̸� �Ѻ��� ���� 256 -> ����ġ�� ũ�� 32 -> 256 / 32 = 8ĭ

	UINT width, height;
	UINT patchWidth, patchHeight;//patch�� ����

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

	float backwardRate = 100.0f;//ī�޶� �ø� ����
};