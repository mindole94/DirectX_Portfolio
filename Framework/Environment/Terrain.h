#pragma once

class Terrain : public Renderer
{
public:
	Terrain(Shader* shader, wstring file);
	~Terrain();

	void Update();
	void Render();

	void BaseMap(wstring file);

	float GetHeight(Vector3& position);
	float GetVerticalRaycast(Vector3& position);
	Vector3 GetRaycastPosition();

	void RaiseHeight(Vector3& center, float intensity);
	void PaintHeight(UINT layer, Vector3& center, float intensity);

	void BrushLocation(Vector3& position) { brushDesc.Location = position; }
	void BrushType(UINT type) { brushDesc.Type = type; }
	void BrushRange(UINT range) { brushDesc.Range = range; }

	void VisibleLine(bool val) { lineDesc.Visible = val ? 1 : 0; }
	void LineColor(Color& color) { lineDesc.Color = color; }
	void LineThickness(float val) { lineDesc.Thickness = val; }
	void LineSize(float val) { lineDesc.Size = val; }

	void SaveXmlFile();

private:
	void LoadXmlFile(wstring file);

	void CreateVertexData();
	void CreateIndexData();
	void CreateNormalData();

private:
	struct BrushDesc
	{
		Color Color = D3DXCOLOR(0, 1, 0, 1);
		Vector3 Location;
		UINT Type = 0;
		UINT Range = 2;
		float Padding[3];
	} brushDesc;

	struct LineDesc
	{
		Color Color = D3DXCOLOR(1, 1, 1, 1);
		UINT Visible = 0;
		float Thickness = 0.01f;
		float Size = 5.0f;
		float Padding;
	} lineDesc;

private:
	struct VertexTerrain
	{
		Vector3 Position;
		Vector2 Uv;
		Vector3 Normal = Vector3(0, 0, 0);
		Vector3 Tangent = Vector3(0, 0, 0);
		Color Color;
	};

public:
	static const wstring DataFolder;

private:
	wstring xmlFile;
	Material* material;

	UINT width, height;

	wstring heightFile;
	float* heights;
	float heightScale = 10.0f;

	VertexTerrain* vertices;
	UINT* indices;

	Texture* baseMap = NULL;

	ConstantBuffer* brushBuffer;
	ID3DX11EffectConstantBuffer* sBrushBuffer;

	ConstantBuffer* lineBuffer;
	ID3DX11EffectConstantBuffer* sLineBuffer;


private:
	struct Layer
	{
		wstring LayerMapFile;
		Texture* LayerMap;

		wstring AlphaMapFile;
		UINT* Alphas;
		ID3D11Texture2D* Texture;
		ID3D11ShaderResourceView* SRV;
	};
	vector<Layer> layers;

	ID3DX11EffectShaderResourceVariable* sAlphaSRV;
	ID3DX11EffectShaderResourceVariable* sLayerSRV;
};
