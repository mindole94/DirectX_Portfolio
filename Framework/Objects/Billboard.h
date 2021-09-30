#pragma once

class Billboard : public Renderer
{
public:
	Billboard(Shader* shader);
	~Billboard();

	void AddTexture(wstring file);
	void Add(Vector3& position, Vector2& scale, UINT mapIndex);

	void Update();
	void Render();
	void PreRender_Reflection();

	void GetCullingPlanes(Plane* planes);

private:
	struct VertexBillboard
	{
		Vector3 Position;
		Vector2 Scale;
		UINT MapIndex;
	};

private:
	vector<VertexBillboard> vertices;

	vector<wstring> textureFiles;
	TextureArray* textureArray = NULL;
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;

	Plane planes[6];
};